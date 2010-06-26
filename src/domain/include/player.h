#ifndef PLAYER_H_
#define PLAYER_H_

#include <string>
#include <deque>
#include <list>
#include <time.h>

#include <skill.h>
#include <character.h>

namespace domain
{

class Room;
class Player;


class StatePlayer {
protected:
	Player* this_player;

public:
	StatePlayer(Player *p);
	virtual ~StatePlayer();

	virtual void write(gmp::GMP_msg *msg) = 0;
	virtual void send_info() = 0;
};



class StatePlayerNormal : public StatePlayer {
public:
	StatePlayerNormal(Player *p);
	virtual ~StatePlayerNormal();

	void write(gmp::GMP_msg *msg);
	void send_info();
};



class StatePlayerDisconnected : public StatePlayer {
public:
	StatePlayerDisconnected(Player *p);
	virtual ~StatePlayerDisconnected();

	void write(gmp::GMP_msg *msg);
	void send_info();
};


class Player : public Character
{
private:
	int id;
	StatePlayer *state;

	std::deque<gmp::GMP_msg*> msg_to_write;

	/** Son skills que el pj té pendents de comensar a tractar.
	 * Ens aseguren que son skills que el pj poseeix
	 */
	std::deque<Skill*> skills_to_do;
	std::deque<HBSkill*> hb_skills_to_do;

	/** Llista de id's dels skills que poseeix el player
	 */
	std::list<int> skills;

	clock_t hb_token;
	static clock_t HB_TIME;
	bool can_jump;
	std::string temporal_name;


	friend class StatePlayerNormal;
	friend class StatePlayerDisconnected;

public:
	Player(int id, std::string name, Room *r);
	virtual ~Player();

	void change_state_disconnected();
	void push_command(Skill *s);
	void push_hb_command(HBSkill *s);
	void add_skill(int n);
	bool has_skill(int id);
	void flush_commands();
	void send_info();
	bool can_quit();
	int get_id();
	void hb_action();
	bool have_hb();

	bool is_player();
	void write(gmp::GMP_msg *msg);
	virtual void die();

	virtual std::string get_name();
	virtual std::string get_real_name();
	void restore_name();
};

}

#endif /*PLAYER_H_*/
