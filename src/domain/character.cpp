#include <character.h>

namespace domain {



Character::~Character() {

}

int Character::get_pvs() {
	return pvs;
}

int Character::get_max_pvs() {
	return max_pvs;
}

int Character::get_pgs() {
	return pgs;
}

void Character::add_combat(Character *c) {
	in_combat.insert(c);
}

void Character::remove_combat(Character *c) {
	in_combat.erase(c);
}

bool Character::is_in_combat() {
	return !in_combat.empty();
}

std::string Character::get_name() {
	return name;
}

void Character::set_name(std::string name) {
	this->name = name;
}

Room* Character::get_room() {
	return room;
}

void Character::set_room(Room *r) {
	room = r;
}

std::set<Character*>* Character::get_in_combat() {
	return &in_combat;
}

void Character::set_damage(int damage) {
	pvs -= damage;
}

bool Character::operator> (Character &c1) {
	return &c1 > this;
}
bool Character::operator<= (Character &c1) {
	return &c1 <= this;
}
bool Character::operator< (Character &c1) {
	return &c1 < this;
}
bool Character::operator>= (Character &c1) {
	return &c1 >= this;
}

bool Character::is_alive() {
	return pvs >= 0;
}

std::string Character::get_real_name() {
	return name;
}

void Character::die() {
	//buida buffers de players in combat, etc...
	in_combat.clear();

	//crea el objecte corpse i se li afegeix l'equip actual del character. Es posa corpse a la room actual
	//TODO: en el cas de mort de un npc aqui hi hauria mes codi, per eliminar el npc i avisar al control de zona que
	// en falta un, perque se n'ocupi de restaurarlo quan toqui

}

}