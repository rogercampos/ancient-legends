#ifndef WORLDCONTROLLER_H_
#define WORLDCONTROLLER_H_

#include <ext/hash_map>
#include <room.h>
#include <zone.h>
#include <character.h>

namespace domain
{

class WorldController
{
protected:
	WorldController();
	~WorldController();
	WorldController(const WorldController&);
	WorldController& operator= (const WorldController&);

private:
	static WorldController *instance;
	__gnu_cxx::hash_map<int, Room*> rooms;
	__gnu_cxx::hash_map<int, Zone*> zones;

	void link_rooms();

public:
	static WorldController* get_instance();
	void make_world();

	Zone* get_zone(int id);
	Room* get_room(int id);

	void broadcast(Room *r, gmp::GMP_msg &msg,
							Character *p = NULL,
							Character *p2 = NULL);
};

}

#endif /*WORLDCONTROLLER_H_*/
