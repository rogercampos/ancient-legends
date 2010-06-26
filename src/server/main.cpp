#include <iostream>
#include <string>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/socket.h>


#include <room.h>
#include <worldController.h>
#include <types.h>
#include <connectionHandler.h>
#include <clientManager.h>
#include <skillsController.h>
#include <playersController.h>
#include <dbManager.h>




/**
 * Crea un proces que es el que rep conexions noves.
 * Retorna un canal de pipe per on es llegeix la informacio que transmet
 * aquet proces.
 */
void create_connection_handler(int &unix_fdd, int &pipe_fdd) {
	int unix_fd[2];
	int pipe_fd[2];

	socketpair(AF_LOCAL, SOCK_DGRAM, 0, unix_fd);
	pipe(pipe_fd);

	if (fork() == 0) {
		close(unix_fd[1]); //el fill fa servir el [0]
		close(pipe_fd[0]);

		net::ConnectionHandler c(unix_fd[0], pipe_fd[1]);
		c.loop();
	}
	close(unix_fd[0]);
	close(pipe_fd[1]);

	int old_flags = fcntl(unix_fd[1], F_GETFL, 0);
	int r = fcntl(unix_fd[1], F_SETFL,  old_flags | O_NONBLOCK); //no bloquejant
	if (r < 0) {
		std::perror("Error en el fcntl()");
		exit(-1);
	}

	old_flags = fcntl(pipe_fd[0], F_GETFL, 0);
	r = fcntl(pipe_fd[0], F_SETFL,  old_flags | O_NONBLOCK); //no bloquejant
	if (r < 0) {
		std::perror("Error en el fcntl()");
		exit(-1);
	}
	unix_fdd = unix_fd[1];
	pipe_fdd = pipe_fd[0];
}


int main() {

	int unix_sock_fd, pipe_fd;
	// Inicialitza el proces connectionhandler i torna els dos fd per comunicacio (es pasen per referencia)
	create_connection_handler(unix_sock_fd, pipe_fd);

	// Aqui aniria tota la part de creacio del mon
	// Conectem amb la bd
	db::DBManager::get_instance()->init_connection();

	domain::WorldController::get_instance()->make_world();
	domain::SkillsController::get_instance()->init_skills();

	/*****************************************************
	 *
	 * FI dE LA CREACIO DEL MON
	 *
	 **************************************************/

	int FPS = 50;


	//Inicialitza el ClientManager amb els fds anteriors
	net::ClientManager::get_instance()->init(unix_sock_fd, pipe_fd);

	while(1) {
		net::ClientManager::get_instance()->accept_new_clients();

		// Llegeix info rebuda dels clients i guarda les accions valides rebudes en els seus
		// respectius buffers d'entrada
		net::ClientManager::get_instance()->read_clients();

		// Resoldre comandes rebudes dels clients, que estaran guardades als seus buffers d'entrada
		domain::PlayersController::get_instance()->execute_commands();

		//Es fan les accions de hb dels jugadors
		domain::PlayersController::get_instance()->hb();

		// Elimina de veritat els players que s'haguin de treure
		domain::PlayersController::get_instance()->remove_players();

		// Game logic, s'escriu la informacio a enviar als clients en els seus buffers

		// Escribim info als clients, flush dels seus buffers de sortida
		domain::PlayersController::get_instance()->write();

		nanosleep(10);
	}
}



