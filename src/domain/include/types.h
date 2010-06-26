#ifndef TYPES_H_
#define TYPES_H_

#include <string>

class Direction
{
public:
	static const short int NORTH = 0;
	static const short int NORTHEAST = 1;
	static const short int EAST = 2;
	static const short int SOUTHEAST = 3;
	static const short int SOUTH = 4;
	static const short int SOUTHWEST = 5;
	static const short int WEST = 6;
	static const short int NORTHWEST = 7;

	Direction(short int dir);
	virtual ~Direction();

	static Direction str2card(std::string e);
	Direction reverse();
	std::string get_goto_description();
	std::string get_from_description();

	short int operator()();
	bool operator==(short int v);

private:
	short int dir;
};

#endif /*TYPES_H_*/
