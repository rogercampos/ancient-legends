#ifndef ROOM_H_
#define ROOM_H_

#include <string>
#include <set>


#include <character.h>
#include <zone.h>
#include <types.h>

namespace domain
{

class Room
{
private:
	int id;
	std::string name;
	std::string desc;
	Room *n;
	Room *ne;
	Room *e;
	Room *se;
	Room *s;
	Room *so;
	Room *o;
	Room *no;
	Zone *zone;


	std::set<Character*> characters_in;

public:
	Room(int id, Zone *z);
	Room(int id, std::string name, std::string desc, Zone *z);
	virtual ~Room();

	/** Funcio per afegir un nou player en aquesta room */
	void add_character(Character *p);
	void remove_character(Character *p);

	// Retorna un punter al vector de players
	std::set<Character*>* get_characters();

	Character* get_character_by_name(std::string name);

	int valid_exits();

	Room* get_exit(Direction e);
	Room* get_exit(std::string e);
	void add_exit(Direction e, Room *r);
	void add_exit(std::string e, Room *r);
	bool has_exit(Direction);
	bool has_exit(std::string e);


	std::string get_name() {
		return name;
	}

	void set_name(std::string d) {
		name = d;
	}

	std::string get_desc() {
		return desc;
	}

	void set_desc(std::string d) {
		desc = d;
	}

	Zone* get_zone() {
		return zone;
	}
};

}

#endif /*ROOM_H_*/
