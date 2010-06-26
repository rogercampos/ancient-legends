#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <algorithm>

#include <mud_error.h>
#include <clientManager.h>
#include <playersController.h>
#include <gmp.h>

namespace net {


ClientManager* ClientManager::instance = 0;

ClientManager* ClientManager::get_instance() {
	if (instance == 0) {
		instance = new ClientManager;
	}
	return instance;
}

ClientManager::ClientManager()
{
}

ClientManager::~ClientManager()
{
}


void ClientManager::init(int unix_fd, int pipe_fd) {
	this->unix_fd = unix_fd;
	this->pipe_fd = pipe_fd;
	__tv.tv_sec = 0;
	__tv.tv_usec = 0;

	max_client_fd = -1;
}


int ClientManager::recv_fd() {
	struct msghdr msg;
	struct iovec iov;
	char buf[1];
	int rv;
	int connfd = -1;
	char ccmsg[CMSG_SPACE(sizeof(connfd))];
	struct cmsghdr *cmsg;

	iov.iov_base = buf;
	iov.iov_len = 1;

	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	/* old BSD implementations should use msg_accrights instead of
	 * msg_control; the interface is different. */
	msg.msg_control = ccmsg;
	msg.msg_controllen = sizeof(ccmsg); /* ? seems to work... */

	rv = recvmsg(unix_fd, &msg, 0);
	if (rv == -1) {
		mud_perror("Error en el recvmsg()");
	}

	cmsg = CMSG_FIRSTHDR(&msg);

	if (cmsg->cmsg_type != SCM_RIGHTS) {
	  	mud_error("got control message of unknown type");
	}
	return *(int*)CMSG_DATA(cmsg);

}

/** Funcio que verifica si hi ha algo per llegir en la pipe de comunicacio amb
 * el ConnectionHandler, si es llegeix algo s'afegeix el nou client
 */
void ClientManager::accept_new_clients() {
	FD_ZERO(&read_set);
	FD_SET(unix_fd, &read_set);
	int res = select(unix_fd+1, &read_set, NULL, NULL, &__tv);
	if (res < 0) {
		mud_perror("Error de select()");
	}
	if (res > 0) {
		//En aquest punt hem rebut algo per unix_fd, i ConnectionHandler ens asegura
		//que primer ens envia el ClientConnection per la pipei despres el fd del socket
		//pel unix_fd, aixi que podem asegurar que tindrem algo per llegir a la pipe.
		// Es una forma simple de sincronitzacio
		ClientConnection *c = new ClientConnection;
		read(pipe_fd, c, sizeof (*c));

		int clifd = recv_fd();
		int old_flags = fcntl(clifd, F_GETFL, 0);
		int r = fcntl(clifd, F_SETFL,  old_flags | O_NONBLOCK); //no bloquejant
		if (r < 0) mud_perror("Error en el fcntl()");

		// Hem d'actualitzar el numeret de fd del socket. El clientConnection que hem
		// rebut tenia el fd del ConnectionHandler que sera, segurament, diferent del
		// fd que ens ha assignat al nostre proces el recv_fd()
		c->set_sockfd(clifd);

		/** pot haber conectat un client que ja te una
		 * conexio oberta i activa. Si es el cas, s'haura de descartar l'anterior
		 * conexio (close) i substituirla per la nova
		 */
		if (clients.find(c->get_player()) != clients.end()) {
			write(clifd, "Reconectado\n", 12);
			write(clients[c->get_player()]->get_sockfd(), "Has iniciado sesion en otro lugar.\n", 35);
			disconnect_client(c->get_player());
		}
		else {
			domain::PlayersController::get_instance()->create_player(c->get_player());
		}

		std::cout << "Client conectat amb id: " << c->get_player() << std::endl;

		clients[c->get_player()] = c;
		client_sockets.push_front(clifd);
		players_ids[clifd] = c->get_player();

		if (clifd > max_client_fd) max_client_fd = clifd;
	}
}

/**
 * Desconecta un client a partir del id player
 */
void ClientManager::disconnect_client(int player_id) {
	std::cout << "S'ha pirat un client" << std::endl;
	int fd = clients[player_id]->get_sockfd();
	client_sockets.remove(fd);
	delete clients[player_id];

	clients.erase(player_id);
	players_ids.erase(fd);

	if (fd == max_client_fd) {
		if (client_sockets.empty()) {
			// Hem eliminat l'ultim client, ara no hi ha ningu conectat
			max_client_fd = -1;
		}
		else {
			// TODO: buscar el màxim de tots els valors de la llista client_sockets i posarlo a max_client_fd

		}
	}

	close(fd);
}

void ClientManager::remove_clients_as_needed() {
	std::list<int>::iterator it;
	for (it = clients_to_remove.begin(); it != clients_to_remove.end(); ++it) {
		// disconnect_client(*it);
		domain::PlayersController::get_instance()->maybe_remove_player(
				domain::PlayersController::get_instance()->get_player(*it));
	}
	clients_to_remove.clear();
}

void ClientManager::read_clients() {
	// Si no hi ha clients conectats, torna
	if (max_client_fd < 0) return;

	FD_ZERO(&read_set);
	std::list<int>::iterator it;
	for (it = client_sockets.begin(); it != client_sockets.end(); ++it) {
		FD_SET(*it, &read_set);
	}

	int res = select(max_client_fd+1, &read_set, NULL, NULL, &__tv);
	if (res < 0) mud_perror("Error en el select()");
	if (res > 0) {
		for (it = client_sockets.begin(); it != client_sockets.end(); ++it) {
			if (FD_ISSET(*it, &read_set)) {
				parse_input(*it);
			}
		}
		remove_clients_as_needed();
	}

}

/**
 * Llegeix la info que s'ha rebut del client pel canal fd i
 * la transmet al nivell superior.
 * TODO: Com que estem en TCP, podria ser que ens arribesin dos comandos
 * en una mateixa lectura. Detectaro !!
 * - Amb un client telnet no podria pasar mai, pero si podria pasar amb un
 * client fet per nosaltres, en algun cas.
 */
void ClientManager::parse_input(int fd) {
	/** TODO: S'hauria d'augmentar el tamany d'aquest buffer de recepcio.
	 * Com que estem en TCP, seria posible rebre dos misatges del client
	 * ajuntats en una sola lectura! Controlaro
	 * Amb telnet segur que no pasa, pero amb un client fet per nosaltres
	 * podria pasar, mes si es grafic.
	 */
	char buff[256];
	int readed = read(fd, buff, 256);

	//Si algu s'ha desconectat llegirem 0 bytes
	if (readed == 0) {
		clients_to_remove.push_front(players_ids[fd]);
		return;
	}

	if (readed == 256) {
		/* TODO:
		 * GMP_msg msg = GMP_Protocol::get_instance()->get_overflow_err_msg();
		 * Aquests serien un tipus escpecifis de msg per a errors.
 		 * enviar msg al client.
		 */
	}

	/*
	 * Aquesta funcio de parse, nomes comprova si el que s'ha rebut es algo valid o no.
	 * Si no es valid, envia inmediatament un msg de error al client. Si es valid,
	 * afegeix el comando arealitzar a la pila d'aquell player. PERO NO ES RESOL
	 * ARA. Les comandes rebudes es resolen totes juntes despres
	 */
	gmp::GMP_Procotol::get_instance()->parse(players_ids[fd], buff, readed);
}

void ClientManager::send_to_client(int player_id, gmp::GMP_msg *msg) {
	//envia el misatge donat al client
	ClientConnection *c = clients[player_id];
	msg->finalize();
	write(c->get_sockfd(), msg->get_msg(), msg->get_len());

}

void ClientManager::remove_client(int player_id) {
	disconnect_client(player_id);
}

}
