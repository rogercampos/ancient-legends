#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <mysql++/mysql++.h>

#include <worldController.h>
#include <dbManager.h>
#include <player.h>
namespace domain
{

WorldController* WorldController::instance = 0;

WorldController* WorldController::get_instance() {
	if (instance == 0) {
		instance = new WorldController;
	}
	return instance;
}

WorldController::WorldController()
{
}

WorldController::~WorldController()
{
/** - Eliminar les zones
 *  - Eliminar les rooms
 */
	for (__gnu_cxx::hash_map<int, Zone*>::iterator i=zones.begin(); i!=zones.end(); ++i) {
		delete i->second;
	}
	for (__gnu_cxx::hash_map<int, Room*>::iterator i=rooms.begin(); i!=rooms.end(); ++i) {
		delete i->second;
	}
}


void WorldController::make_world() {
	mysqlpp::StoreQueryResult res;
	db::DBManager::get_instance()->get_zones(res);

    for (size_t i = 0; i < res.num_rows(); ++i) {
    	Zone *z = new Zone(res[i][0], std::string(res[i][1].c_str()), std::string(res[i][2].c_str()));
    	zones[res[i][0]] = z;
    }

    res.clear();
    db::DBManager::get_instance()->get_rooms(res);

    for (size_t i = 0; i < res.num_rows(); ++i) {
    	std::string name;
    	std::string desc;
    	if (res[i][2] == "NULL") {
    		name = "";
    	}
    	else {
    		name = std::string(res[i][2].c_str());
    	}
    	if (res[i][3] == "NULL") {
    		desc = "";
    	}
    	else {
    		desc = std::string(res[i][3].c_str());
    	}

    	Room *r = new Room(res[i][0], name, desc, zones[res[i][1]]);
    	rooms[res[i][0]] = r;

    	/**
    	 * Utilitzem un *petit hack* aqui.
    	 * 1) Problema a l'hora de crear totes les rooms: Per crear una room
    	 * que esta linkada amb un altre, necesitem tenir un punter a aquest
    	 * altre, per tant necesitem que aquesta altre ja estigui feta.
    	 * Que es abans, ou o gallina? => No es pot fer de cop.
    	 * 2) Solucio: primer es creen totes les rooms sense links, i despres
    	 * es linken.
    	 * 3) Problema: en la fase de creacio dels objectes rooms sense links,
    	 * que es l'unica vegada que es llegeix la bd, necesitem guardar la info
    	 * de quins links tenen cada room. Com ho fem?
    	 * 	- Utilitzar estrucutra de dades adicional: seria lo mes correcte,
    	 * 	  pero menys pro.
    	 *  - Aprofitar la clase Room modificanla una mica. Ara guarda un punter
    	 *    per cada sortida que te, un punter a Room. El que necesitem
    	 *    guardar es just aixo, per en comptes de punters a rooms hem de
    	 *    guardar les id's de les rooms.
    	 *    Pero aixi embrutariem la clase Room, que ja esta be.
    	 *  - Aprofitar la clase Room sense modificarla, utilitzan directament
    	 *    els punters a Room per guardar la nostra info (ids de rooms).
    	 *    Al cap davall els punters son ints de 4 bytes, ja ens serveixen.
    	 *    Pero el compilador es queixa una mica si li fiquem un int a pelo
    	 *    sense ser un Room*, per aixo fem el cast.
    	 */
    	if (res[i][4] != mysqlpp::null) {
    		int v = res[i][4];
    		r->add_exit("n", (Room *)v);
    	}
    	if (res[i][5] != mysqlpp::null) {
    		int v = res[i][5];
    		r->add_exit("ne", (Room *)v);
    	}
    	if (res[i][6] != mysqlpp::null) {
    		int v = res[i][6];
    		r->add_exit("e", (Room *)v);
    	}
    	if (res[i][7] != mysqlpp::null) {
    		int v = res[i][7];
    		r->add_exit("se", (Room *)v);
    	}
    	if (res[i][8] != mysqlpp::null) {
    		int v = res[i][8];
    		r->add_exit("s", (Room *)v);
    	}
    	if (res[i][9] != mysqlpp::null) {
    		int v = res[i][9];
    		r->add_exit("so", (Room *)v);
    	}
    	if (res[i][10] != mysqlpp::null) {
    		int v = res[i][10];
    		r->add_exit("o", (Room *)v);
    	}
    	if (res[i][11] != mysqlpp::null) {
    		int v = res[i][11];
    		r->add_exit("no", (Room *)v);
    	}
    }

	link_rooms();

}

Zone* WorldController::get_zone(int id) {
	return zones[id];
}

Room* WorldController::get_room(int id) {
	return rooms[id];
}

/**
 * Afegim les sortides a les rooms (transformem els punters de ser
 * "virtuals" (contenen les ids de les rooms) a ser reals).
 */
void WorldController::link_rooms() {
	for (__gnu_cxx::hash_map<int, Room*>::iterator i=rooms.begin(); i!=rooms.end(); ++i) {
		if (int valor = (int)i->second->get_exit("n")) {
			i->second->add_exit("n", rooms[valor]);
		}
		if (int valor = (int)i->second->get_exit("ne")) {
			i->second->add_exit("ne", rooms[valor]);
		}
		if (int valor = (int)i->second->get_exit("e")) {
			i->second->add_exit("e", rooms[valor]);
		}
		if (int valor = (int)i->second->get_exit("se")) {
			i->second->add_exit("se", rooms[valor]);
		}
		if (int valor = (int)i->second->get_exit("s")) {
			i->second->add_exit("s", rooms[valor]);
		}
		if (int valor = (int)i->second->get_exit("so")) {
			i->second->add_exit("so", rooms[valor]);
		}
		if (int valor = (int)i->second->get_exit("o")) {
			i->second->add_exit("o", rooms[valor]);
		}
		if (int valor = (int)i->second->get_exit("no")) {
			i->second->add_exit("no", rooms[valor]);
		}
	}
}


/**
 * Envia el misatge msg a tots els jugadors d'una room, excepte al
 * jugador "p" i al jugador "p2" (opcional p2)
 */
void WorldController::broadcast(Room *r, gmp::GMP_msg &msg, Character *p, Character *p2) {

	msg.append("\n");

	std::set<Character*>* players = r->get_characters();
	for (std::set<Character*>::iterator i = players->begin();
			i != players->end(); ++i) {
		if ((*i)->is_player()) {
			if (*i != p && *i != p2) {
				((Player*)(*i))->write(msg.clone());
			}
		}
	}
}


}

