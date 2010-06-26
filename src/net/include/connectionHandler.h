#ifndef CONNECTIONHANDLER_H_
#define CONNECTIONHANDLER_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define NEW_CLIENT 1

namespace net
{

/** Aquesta clase s'encarrega d'escoltar pel socket genera d'entrada i acceptar
 * noves conexions. Quant rep una nova peticio de conexio el que *HAURIA* de fer
 * (esta a mitges) es:
 * - crear un thread per gestionar la nova peticio, i continuar escoltan noves entrades
 * - En aquest nou thread, fer:
 * 		- identificar el nou client, amb nom i password
 * 		- informar al client dels jugadors que te
 * 		- rebre del client la info de quin jugador escull per entrar al joc
 * 		- crear un objecte ClientConnection amb aquesta informacio
 * 		- enviar l'objecte per la pipe al main process.
 *		- segurament mes coses, com guardar logs. Info de la hora que sa conectat,
 * 			desde quina IP, si ha fallat intents de password, etc...
 *
 * NOTA: Es podria crear un mini-protocol per dur a terme tota a aquesta feina
 */
class ConnectionHandler
{
private:
	int main_socket;
	int unix_fd;	//unix socket per enviar file descritors dels sockets
	int pipe_fd;	//pipe per transmetre objectes ClientConnection

	std::string fix_input(int len, char *s);

public:
	ConnectionHandler(int unix_fd, int pipe_fd);
	virtual ~ConnectionHandler();

	void loop();

	void send_fd(int fd);
};



/** Representa la conexio d'un client amb el joc.
 * Es guarda el socket del client, la info del socket (Ip basicament del client)
 * i player amb qui jugar
 */
class ClientConnection
{
private:
	int sockfd;
	struct sockaddr_in client_addr;
	int player_id;

public:
	ClientConnection();
	ClientConnection(int fd, struct sockaddr_in *client_addr, int player_id);
	virtual ~ClientConnection();

	int get_sockfd();
	in_addr get_addr();
	int get_player();

	void set_sockfd(int fd);
	void set_player_id(int id);
};

}

#endif /*CONNECTIONHANDLER_H_*/

