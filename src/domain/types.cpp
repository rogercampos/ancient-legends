#include <types.h>


Direction::Direction(short int dir) {
	this->dir = dir;
}

Direction::~Direction() {
}

Direction Direction::str2card(std::string e) {
	if (e == "n") {
		return Direction(NORTH);
	}
	else if (e == "ne") {
		return Direction(NORTHEAST);
	}
	else if (e == "e") {
		return Direction(EAST);
	}
	else if (e == "se") {
		return Direction(SOUTHEAST);
	}
	else if (e == "s") {
		return Direction(SOUTH);
	}
	else if (e == "so") {
		return Direction(SOUTHWEST);
	}
	else if (e == "o") {
		return Direction(WEST);
	}
	else if (e == "no") {
		return Direction(NORTHWEST);
	}
	return 0;
}

short int Direction::operator()() {
	return dir;
}

bool Direction::operator==(short int v) {
	return this->dir == v;
}

Direction Direction::reverse() {
	if (dir == NORTH) {
		return Direction(SOUTH);
	}
	else if (dir == NORTHEAST) {
		return Direction(SOUTHWEST);
	}
	else if (dir == EAST) {
		return Direction(WEST);
	}
	else if (dir == SOUTHEAST) {
		return Direction(NORTHWEST);
	}
	else if (dir == SOUTH) {
		return Direction(NORTH);
	}
	else if (dir == SOUTHWEST) {
		return Direction(NORTHEAST);
	}
	else if (dir == WEST) {
		return Direction(EAST);
	}
	else if (dir == NORTHWEST) {
		return Direction(SOUTHEAST);
	}
}

std::string Direction::get_goto_description() {
	if (dir == NORTH) {
		return "el norte";
	}
	else if (dir == NORTHEAST) {
		return "el noreste";
	}
	else if (dir == EAST) {
		return "el este";
	}
	else if (dir == SOUTHEAST) {
		return "el sudeste";
	}
	else if (dir == SOUTH) {
		return "el sur";
	}
	else if (dir == SOUTHWEST) {
		return "el suroeste";
	}
	else if (dir == WEST) {
		return "el oeste";
	}
	else if (dir == NORTHWEST) {
		return "el noroeste";
	}
}

std::string Direction::get_from_description() {
	if (dir == NORTH) {
		return "del norte";
	}
	else if (dir == NORTHEAST) {
		return "del noreste";
	}
	else if (dir == EAST) {
		return "del este";
	}
	else if (dir == SOUTHEAST) {
		return "del sudeste";
	}
	else if (dir == SOUTH) {
		return "del sur";
	}
	else if (dir == SOUTHWEST) {
		return "del suroeste";
	}
	else if (dir == WEST) {
		return "del oeste";
	}
	else if (dir == NORTHWEST) {
		return "del noroeste";
	}
}
