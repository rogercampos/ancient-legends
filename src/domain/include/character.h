#ifndef CHARACTER_H_
#define CHARACTER_H_

#include <set>
#include <string>

#include <GMP_msg.h>


namespace domain {

class Room;

class Character {
protected:
	std::set<Character*> in_combat;
	std::string name;
	Room *room;

	int pvs;
	int max_pvs;
	int pgs;


public:
	Character(std::string name, Room *r, int pv=300, int pg=300) :
		name(name), room(r), pvs(pv), pgs(pg)
	{
		max_pvs = 300;
	}

	bool operator> (Character &c1);
    bool operator<= (Character &c1);
    bool operator< (Character &c1);
    bool operator>= (Character &c1);

	virtual ~Character();

	int get_pvs();
	int get_pgs();

	int get_max_pvs();

	void add_combat(Character *c);
	void remove_combat(Character *c);
	bool is_in_combat();

	virtual std::string get_name();
	virtual std::string get_real_name();
	void set_name(std::string name);

	virtual bool is_player() = 0;

	Room* get_room();
	void set_room(Room *r);

	std::set<Character*>* get_in_combat();

	void set_damage(int d);
	bool is_alive();

	virtual void die();
};

}

#endif /*CHARACTER_H_*/
