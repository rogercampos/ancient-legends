#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <mysql++/mysql++.h>
#include <list>
#include <iostream>

#include <playersController.h>
#include <skillsController.h>
#include <worldController.h>
#include <room.h>
#include <clientManager.h>
#include <dbManager.h>

namespace domain
{

PlayersController* PlayersController::instance = 0;

PlayersController* PlayersController::get_instance() {
	if (instance == 0) {
		instance = new PlayersController;
	}
	return instance;
}

PlayersController::PlayersController()
{
}

PlayersController::~PlayersController()
{
	for (__gnu_cxx::hash_map<int, Player*>::iterator i=players.begin();
		i!=players.end(); ++i) {
		delete i->second;
	}
}

void PlayersController::create_player(int id) {
	mysqlpp::StoreQueryResult res;
	db::DBManager::get_instance()->get_player(id, res);
	Player *p = new Player(id, std::string(res[0][1].c_str()), NULL);
	init_player(p);
	players[id] = p;
	players_by_name[p->get_real_name()] = p;
	// Situem el pj en la room que tenia
	// TODO: S'hauria de fer alguna comprovacio mes, si la room que te asignada
	// no existeix, portarlo a algun altre lloc. Aqui donem per fet que existeix
	Room *r = WorldController::get_instance()->get_room(res[0][3]);
	p->set_room(r);
	r->add_character(p);

	//Fem que faci un glance al entrar al joc
	p->push_command(domain::SkillsController::get_instance()->get_skill_by_name("glance"));
}

Player* PlayersController::get_player(int id) {
	// find() et torna un iterador == end() si no hi es
	if (players.find(id) != players.end()) {
		return players[id];
	}
	return NULL;
}

Player* PlayersController::get_player(std::string name) {
	// find() et torna un iterador == end() si no hi es
	if (players_by_name.find(name) != players_by_name.end()) {
		return players_by_name[name];
	}
	return NULL;
}


/**
 * Afegim les skills al personatge recien creat. Es fa el seguent:
 * - S'afegeixen lesskills normals per a ser jugador i personatge.
 * - S'afegeix la resta de skills relacionades amb el guild
 * - Altres ...
 */
void PlayersController::init_player(Player *p) {
	std::list<int> aux;
	SkillsController::get_instance()->get_character_skills_ids(aux);
	SkillsController::get_instance()->get_player_skills_ids(aux);

	std::list<int>::iterator i;
	for (i = aux.begin(); i != aux.end(); ++i) {
		p->add_skill(*i);
	}
	// TODO: hauriem danar a bd per afegirli la resta de skills (les de guild que li toquin per lvl + altres)
}

/**
 * Recorre tots els players i executa els seus comandos pendents.
 * TODO: Aqui potser hauria d'anar mes codi, no ho se. L'ordre en que
 * es resolen les accions de diferents players ve donada per la sort.
 * S'hauria de controlar algun ordre determinat?
 * Pensar que aixo de totes formes es nomes per una volta de bucle, el
 * temps real d'execucio d'accions vindradonat pel HB
 */
void PlayersController::execute_commands() {
	for (__gnu_cxx::hash_map<int, Player*>::iterator i=players.begin();
		i!=players.end(); ++i) {
		i->second->flush_commands();
	}
}



/**
 * Recorre tots els players i executa els seus comandos pendents
 */
void PlayersController::write() {
	for (__gnu_cxx::hash_map<int, Player*>::iterator i=players.begin();
		i!=players.end(); ++i) {
		i->second->send_info();
	}
}

/**
 * Elimina un jugador del joc. L'elimina en tots els sentits, excepte treurel
 * de la llista players interna. (perque aixo sexecutara en un for sobre aquesta
 * llista)
 */
void PlayersController::remove_player(Player *p) {
	players_to_remove.push_back(p);

	//Informem que l'usuari surt
	gmp::GMP_msg msg(p->get_name()+" se ha desconectado");
	WorldController::get_instance()->broadcast(p->get_room(), msg, p);

	// TODO: Aqui s'elimina efectivament el player del joc, es a dir,
	// es borra tota referencia que pugui existir cap aquest jugador
	p->get_room()->remove_character(p);

	net::ClientManager::get_instance()->remove_client(p->get_id());
}

/**
 * Funcio que es crida per intentar eliminar un Player. Es cridada
 * desde la capa de red quant es detecta que el client ha tancat o ha perdut
 * la conexio.
 * Si no pot sortir del joc, es converteix en estatua lagueada.
 */
void PlayersController::maybe_remove_player(Player *p) {
	if (p->can_quit()) remove_player(p);
	else {
		p->change_state_disconnected();
		net::ClientManager::get_instance()->remove_client(p->get_id());
	}
}

/**
 * Borra els players que toqui de la llista interna players
 */
void PlayersController::remove_players() {
	std::vector<Player *>::iterator it;
	for (it = players_to_remove.begin(); it != players_to_remove.end(); ++it) {
		players.erase((*it)->get_id());
		players_by_name.erase((*it)->get_real_name());
		delete (*it);
	}

	players_to_remove.clear();
}

void PlayersController::hb() {
	for (__gnu_cxx::hash_map<int, Player*>::iterator i=players.begin();
		i!=players.end(); ++i) {
		i->second->hb_action();
	}
}


}
