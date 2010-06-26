#ifndef CLIENTMANAGER_H_
#define CLIENTMANAGER_H_

#include <sys/types.h>
#include <list>
#include <ext/hash_map>


#include <connectionHandler.h>
#include <GMP_msg.h>

namespace net {

class ClientManager {

protected:
	ClientManager();
	~ClientManager();
	ClientManager(const ClientManager&);
	ClientManager& operator= (const ClientManager&);

private:
	static ClientManager *instance;

	// Per fer el polling de la pipe amb ConnectionHandler, i tambe dels sockets
	// dels diferents clients
	struct timeval __tv;
	fd_set read_set;

	int unix_fd;
	int pipe_fd;

	/** Hash per relacionar el player_id amb el objecte ClientConnection.
	 * Es fa servir per quant hem d'enviar misatges a determinats players
	 */
	__gnu_cxx::hash_map<int, ClientConnection*> clients;

	/** Llista de sockets dels clients. Es fa servir per teniri un rapid acces
	 * a l'hora de fer un broadcast, i sobretot per llegirlos tots de cop
	 */
	std::list<int> client_sockets;

	/** Hash que relaciona el socket fd amb el player id. Es per poder
	 * identificar qui ha enviat un misatge, quant el rebem nomes sabem el fd
	 * pel qual l'hem rebut
	 */
	__gnu_cxx::hash_map<int, int> players_ids;

	/** Llista on guardem els player_id dels clients desconectats.
	 * S'han de guardar nomes temporalment, fins que acabi el recorregut
	 * sobre tots ells, per despres treure els desconectats. No es pot fer
	 * insitu perque seria cambiar el tamany sobre el questas fent e lfor
	 */
	std::list<int> clients_to_remove;

	// El màxim de tots els fd dels sockets dels clients
	int max_client_fd;


	int recv_fd();
	void disconnect_client(int fd);
	void parse_input(int fd);
	void remove_clients_as_needed();

public:
	static ClientManager* get_instance();

	void init(int unix_fd, int pipe_fd);

	void accept_new_clients();
	void read_clients();

	void send_to_client(int player_id, gmp::GMP_msg *msg);

	void remove_client(int player_id);

};



}

#endif /*CLIENTMANAGER_H_*/
