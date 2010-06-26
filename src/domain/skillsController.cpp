#include <skillsController.h>
#include <vector>

namespace domain {

SkillsController* SkillsController::instance = 0;

SkillsController* SkillsController::get_instance() {
	if (instance == 0) {
		instance = new SkillsController;
	}
	return instance;
}

SkillsController::SkillsController()
{
}

SkillsController::~SkillsController()
{
	for (__gnu_cxx::hash_map<std::string, Skill*>::iterator i=character_skills.begin();
		i!=character_skills.end(); ++i) {
		delete i->second;
	}

	for (__gnu_cxx::hash_map<std::string, Skill*>::iterator i=player_skills.begin();
		i!=player_skills.end(); ++i) {
		delete i->second;
	}
}

void SkillsController::add_skill(int &id, std::string name, int type, skillfnc handler, int turns) {
	Skill *s;
	if (turns == 0) s = new Skill(id);
	else 			s = new HBSkill(id, turns);

	if (type == SKILL_CHARACTER_TYPE) {
		character_skills[name] = s;
	}
	else if (type == SKILL_PLAYER_TYPE) {
		player_skills[name] = s;
	}
	skill_handlers[id] = handler;
	id++;
}


/**
 * Crea tots els comandos que existeixen en el joc, TOTS
 */
void SkillsController::init_skills() {
	int id = 1;
	add_skill(id, "n", SKILL_CHARACTER_TYPE, &SkillsActions::n);
	add_skill(id, "ne", SKILL_CHARACTER_TYPE, &SkillsActions::ne);
	add_skill(id, "e", SKILL_CHARACTER_TYPE, &SkillsActions::e);
	add_skill(id, "se", SKILL_CHARACTER_TYPE, &SkillsActions::se);
	add_skill(id, "s", SKILL_CHARACTER_TYPE, &SkillsActions::s);
	add_skill(id, "so", SKILL_CHARACTER_TYPE, &SkillsActions::so);
	add_skill(id, "o", SKILL_CHARACTER_TYPE, &SkillsActions::o);
	add_skill(id, "no", SKILL_CHARACTER_TYPE, &SkillsActions::no);
	add_skill(id, "decir", SKILL_CHARACTER_TYPE, &SkillsActions::decir);
	add_skill(id, "matar", SKILL_CHARACTER_TYPE, &SkillsActions::matar);

	add_skill(id, "glance", SKILL_PLAYER_TYPE, &SkillsActions::glance);
	add_skill(id, "sc", SKILL_PLAYER_TYPE, &SkillsActions::sc);
	add_skill(id, "tell", SKILL_PLAYER_TYPE, &SkillsActions::tell);
	add_skill(id, "salir", SKILL_PLAYER_TYPE, &SkillsActions::salir);
	add_skill(id, "ficha", SKILL_PLAYER_TYPE, &SkillsActions::ficha);
	add_skill(id, "peleas", SKILL_PLAYER_TYPE, &SkillsActions::peleas);
	add_skill(id, "stoppeleas", SKILL_PLAYER_TYPE, &SkillsActions::stoppeleas);
	add_skill(id, "tell", SKILL_PLAYER_TYPE, &SkillsActions::tell);

	add_skill(id, "gcritico", SKILL_PLAYER_TYPE, &SkillsActions::gcritico);
	add_skill(id, "golpear", SKILL_PLAYER_TYPE, &SkillsActions::golpear, 1);
	add_skill(id, "formular", SKILL_PLAYER_TYPE, &SkillsActions::formular);

}


/**
 * Ens torna el punter a UNA COPIA del skill demanat. Si el skill
 * no existeix torna null
 */
Skill* SkillsController::get_skill_by_name(std::string name) {
	if (player_skills.find(name) != player_skills.end()) {
		return player_skills[name]->clone();
	}
	else if (character_skills.find(name) != character_skills.end()) {
		return character_skills[name]->clone();
	}

	return NULL;
}

void SkillsController::get_character_skills_ids(std::list<int> &aux) {
	for (__gnu_cxx::hash_map<std::string, Skill*>::iterator i=character_skills.begin();
		i!=character_skills.end(); ++i) {
		aux.push_back(i->second->get_id());
	}
}

void SkillsController::get_player_skills_ids(std::list<int> &aux) {
	for (__gnu_cxx::hash_map<std::string, Skill*>::iterator i=player_skills.begin();
		i!=player_skills.end(); ++i) {
		aux.push_back(i->second->get_id());
	}
}

void SkillsController::execute(Player *p, Skill *s) {
	skillfnc handler = skill_handlers[s->get_id()];
	//skillfnc handler = s->get_handler();
	(SkillsActions::get_instance()->*handler)(p, s);
}

}

