#include <room.h>
#include <algorithm>

#include <iostream>

namespace domain
{

Room::Room(int id, Zone *z) {
	this->id = id;
	zone = z;
	name = "";
	desc = "";
	n = 0;
	ne = 0;
	e = 0;
	se = 0;
	s = 0;
	so = 0;
	o = 0;
	no = 0;
}

Room::Room(int id, std::string name, std::string desc, Zone *z) {
	this->id = id;
	zone = z;
	this->name = name;
	this->desc = desc;
	n = 0;
	ne = 0;
	e = 0;
	se = 0;
	s = 0;
	so = 0;
	o = 0;
	no = 0;
}


Room::~Room() {
}

std::set<Character*>* Room::get_characters() {
	return &characters_in;
}

int Room::valid_exits() {
	int x = 128*(n>0) + 64*(ne>0) + 32*(e>0) + 16*(se>0) + 8*(s>0) + 4*(so>0) + 2*(o>0) + (no>0);
	return x;
}

Room* Room::get_exit(std::string e) {
	return this->get_exit(Direction::str2card(e));
}

Room* Room::get_exit(Direction exit) {
	if (exit == Direction::NORTH) {
		return n;
	}
	else if (exit == Direction::NORTHEAST) {
		return ne;
	}
	else if (exit == Direction::EAST) {
		return e;
	}
	else if (exit == Direction::SOUTHEAST) {
		return se;
	}
	else if (exit == Direction::SOUTH) {
		return s;
	}
	else if (exit == Direction::SOUTHWEST) {
		return so;
	}
	else if (exit == Direction::WEST) {
		return o;
	}
	else if (exit == Direction::NORTHWEST) {
		return no;
	}
	return 0;
}

void Room::add_exit(std::string e, Room *r) {
	add_exit(Direction::str2card(e), r);
}

void Room::add_exit(Direction exit, Room *r) {
	if (exit == Direction::NORTH) {
		n = r;
	}
	else if (exit == Direction::NORTHEAST) {
		ne = r;
	}
	else if (exit == Direction::EAST) {
		e = r;
	}
	else if (exit == Direction::SOUTHEAST) {
		se = r;
	}
	else if (exit == Direction::SOUTH) {
		s = r;
	}
	else if (exit == Direction::SOUTHWEST) {
		so = r;
	}
	else if (exit == Direction::WEST) {
		o = r;
	}
	else if (exit == Direction::NORTHWEST) {
		no = r;
	}
}

bool Room::has_exit(Direction exit) {
	if (exit == Direction::NORTH) {
		return n>0;
	}
	else if (exit == Direction::NORTHEAST) {
		return ne>0;
	}
	else if (exit == Direction::EAST) {
		return e>0;
	}
	else if (exit == Direction::SOUTHEAST) {
		return se>0;
	}
	else if (exit == Direction::SOUTH) {
		return s>0;
	}
	else if (exit == Direction::SOUTHWEST) {
		return so>0;
	}
	else if (exit == Direction::WEST) {
		return o>0;
	}
	else if (exit == Direction::NORTHWEST) {
		return no>0;
	}
	return false;
}

bool Room::has_exit(std::string e) {
	return has_exit(Direction::str2card(e));
}

void Room::add_character(Character *p) {
	characters_in.insert(p);
}

void Room::remove_character(Character *p) {
	characters_in.erase(p);
}

Character* Room::get_character_by_name(std::string name) {
	std::set<Character*>::iterator i;
	for (i=characters_in.begin(); i!=characters_in.end(); ++i) {
		if ((*i)->get_real_name() == name) return *i;
	}
	return NULL;
}

}
