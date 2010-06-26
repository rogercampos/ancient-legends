#include <skill.h>

namespace domain {


Skill::Skill(int id) : id(id) {

}

Skill::Skill(const Skill *c) : id(c->id) {

}

Skill::~Skill() {
}

int Skill::get_id() {
	return id;
}

void Skill::set_id(int id) {
	this->id = id;
}

Skill* Skill::clone() {
	return new Skill(this);
}

std::string Skill::get_param() {
	return param;
}

void Skill::set_param(std::string s) {
	param = s;
}

bool Skill::is_in_hb() {
	return false;
}


HBSkill::HBSkill(int id, int turns) : Skill(id), turns(turns), actual_turn(0) {

}

HBSkill::HBSkill(const HBSkill *c) : Skill(c->id), turns(c->turns), actual_turn(0) {

}

HBSkill::~HBSkill() {
}

HBSkill* HBSkill::clone() {
	return new HBSkill(this);
}

bool HBSkill::end() {
	return actual_turn == turns;
}

void HBSkill::pass_hb() {
	actual_turn++;
}

int HBSkill::get_turn() {
	return actual_turn;
}

int HBSkill::get_turns() {
	return turns;
}

bool HBSkill::is_in_hb() {
	return true;
}

}
