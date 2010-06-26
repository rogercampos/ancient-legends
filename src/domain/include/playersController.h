#ifndef PLAYERSCONTROLLER_H_
#define PLAYERSCONTROLLER_H_

#include <ext/hash_map>
#include <vector>

#include <player.h>
#include <string_hash.h>


namespace domain
{

class PlayersController
{
protected:
	PlayersController();
	~PlayersController();
	PlayersController(const PlayersController&);
	PlayersController& operator= (const PlayersController&);

private:
	static PlayersController *instance;
	__gnu_cxx::hash_map<int, Player*> players;
	__gnu_cxx::hash_map<std::string, Player*> players_by_name;

	std::vector<Player *> players_to_remove;

	void init_player(Player *p);

public:
	static PlayersController* get_instance();

	void create_player(int id);
	Player* get_player(int id);
	Player* get_player(std::string);

	/** Executa tots els skills pendents de tots els players.
	 * Despres de ser executat, la instancia del skill es eliminada (delete)
	 * i la llista netejada.
	 */
	void execute_commands();

	/** Escriu tots els msgs pendents de ser enviats a tots els players
	 */
	void write();
	void remove_player(Player *p);
	void maybe_remove_player(Player *p);
	void remove_players();
	void hb();
};

}

#endif /*PLAYERSCONTROLLER_H_*/
