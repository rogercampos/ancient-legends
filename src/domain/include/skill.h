#ifndef SKILL_H_
#define SKILL_H_


#include <string>

#define 	SKILL_PLAYER_TYPE		0
#define 	SKILL_CHARACTER_TYPE	1



namespace domain{

class SkillsController;

class Skill
{
protected:
	int id;
	std::string param;


public:
	Skill(const Skill *c);
	Skill(int id);
	virtual ~Skill();

	int get_id();
	void set_id(int id);
	Skill *clone();
	std::string get_param();
	void set_param(std::string p);
	bool is_in_hb();
};

class HBSkill : public Skill
{
private:
	int turns;
	int actual_turn;

public:
	HBSkill(const HBSkill *s);
	HBSkill(int id, int turns);
	virtual ~HBSkill();

	HBSkill *clone();
	bool end();
	void pass_hb();
	int get_turn();
	int get_turns();
	bool is_in_hb();
};
}

#endif /*SKILL_H_*/
