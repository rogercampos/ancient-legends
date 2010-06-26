#ifndef SKILLSCONTROLLER_H_
#define SKILLSCONTROLLER_H_

#include <ext/hash_map>
#include <list>

#include <skill.h>
#include <skillsActions.h>

#include <string_hash.h>


namespace domain
{

typedef void (SkillsActions::*skillfnc)(Player *p, Skill *s);

class Player;

class SkillsController
{
protected:
	SkillsController();
	~SkillsController();
	SkillsController(const SkillsController&);
	SkillsController& operator= (const SkillsController&);

private:
	static SkillsController *instance;
	__gnu_cxx::hash_map<std::string, Skill*> character_skills;
	__gnu_cxx::hash_map<std::string, Skill*> player_skills;


	// Mapeja els id's dels skills amb la seva funcio
	__gnu_cxx::hash_map<int, skillfnc> skill_handlers;

	void add_skill(int &id, std::string name, int type, skillfnc func, int turns=0);

public:
	static SkillsController* get_instance();


	void init_skills();
	Skill* get_skill_by_name(std::string name);

	void get_character_skills_ids(std::list<int> &aux);
	void get_player_skills_ids(std::list<int> &aux);

	void execute(Player *p, Skill *s);
};

}


#endif /*SKILLSCONTROLLER_H_*/
