#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <iostream>

#include <mud_error.h>
#include <connectionHandler.h>
#include <dbManager.h>

namespace net {

ConnectionHandler::ConnectionHandler(int unix_fd, int pipe_fd) {
	this->unix_fd = unix_fd;
	this->pipe_fd = pipe_fd;

	main_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (main_socket < 0)
		mud_perror("Error en crear el socket principal");
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;

	int port = 4000;

	do {
		server_addr.sin_port = htons(port);
		port++;
	} while(bind(main_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0);
	std::cout << "Server inciat al port " << port-1 << std::endl;

	listen(main_socket, 1);
	db::DBManager::get_instance()->init_connection();
}


ConnectionHandler::~ConnectionHandler() {
}


void ConnectionHandler::send_fd(int fd)
{

  struct msghdr msg = {0};
  char ccmsg[CMSG_SPACE(sizeof(fd))];
  struct cmsghdr *cmsg;
  //struct iovec vec;  /* stupidity: must send/receive at least one byte */
  //char *str = (char*)"x";
  int rv;

  //vec.iov_base = str;
  //vec.iov_len = 1;
  //msg.msg_iov = &vec;
  //msg.msg_iovlen = 1;

  /* old BSD implementations should use msg_accrights instead of
   * msg_control; the interface is different. */
  msg.msg_control = ccmsg;
  msg.msg_controllen = sizeof(ccmsg);
  cmsg = CMSG_FIRSTHDR(&msg);
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
  *(int*)CMSG_DATA(cmsg) = fd;
  msg.msg_controllen = cmsg->cmsg_len;

  //msg.msg_flags = 0;

  rv = sendmsg(unix_fd, &msg, 0);
  if (rv < 0) {
	  mud_perror("Error del sendmsg()");
  }
}


void ConnectionHandler::loop() {

	struct sockaddr_in client_addr;
	int clientsockfd;
	socklen_t client_len;
	char s[10];

	while(1) {
		client_len = sizeof(client_addr);
		bzero(&client_addr, client_len);
		clientsockfd = accept(main_socket,
				    		  (struct sockaddr *) &client_addr,
				    		  &client_len);

		if (clientsockfd < 0)
			mud_perror("error en el accept()");

		/* Aqui vindria bastant mes codi, tota la part d'interaccio amb el client.
		 * - fer login, nom de conta + password, autentificacio
		 * - seleccio de personatge
		 * Finalment es crea un objecte ClientConnection complet i s'envia al proces
		 * principal que el comensara a controlar.
		 * OJO!: Tot aixo s'hauria de fer en un thread a part, perque sino mentrestan
		 * dura aquet proces no es podrien acceptar noves conexions
		 */
		write(clientsockfd, "Player name?: ", 14);


		int len = read(clientsockfd, s, 30);

		std::string str = fix_input(len, s);
		int p_id = db::DBManager::get_instance()->login_procedure(str, "");

		if (p_id < 0) {
			write(clientsockfd, "Nop!\n", 5);
			close(clientsockfd);

			continue;
		}

		ClientConnection c(clientsockfd, &client_addr, p_id);

		// Enviem primer el ClientConnection
		write(pipe_fd, &c, sizeof(c));

		//Ara enviem el socket creat al clientManager
		send_fd(clientsockfd);

		// Ja no necesitem nosaltres el socket del client nou
		close(clientsockfd);
	}
}

std::string ConnectionHandler::fix_input(int len, char *s) {
	std::string res = std::string(s, len);
	res = res.substr(0, res.size()-2);

	return res;
}


ClientConnection::ClientConnection(int fd, struct sockaddr_in *client, int player_id) {
	sockfd = fd;
	this->player_id = player_id;
	/** S'ha de copiar el struct sencer per despres poder pasar el objecte
	 * directament per la pipe. Es per facilitar la vida.
	 */
	memcpy(&client_addr, client, sizeof(*client));
}

ClientConnection::ClientConnection() {
	sockfd = -1;
}

ClientConnection::~ClientConnection() {
}

int ClientConnection::get_sockfd() {
	return sockfd;
}

void ClientConnection::set_sockfd(int fd) {
	sockfd = fd;
}

void ClientConnection::set_player_id(int id) {
	this->player_id = id;
}

in_addr ClientConnection::get_addr() {
	return client_addr.sin_addr;
}

int ClientConnection::get_player() {
	return player_id;
}

}
