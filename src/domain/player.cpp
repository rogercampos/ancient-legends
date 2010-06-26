#include <player.h>
#include <room.h>
#include <clientManager.h>
#include <skillsController.h>
#include <clientManager.h>
#include <combatController.h>

#include <algorithm>

#include <iostream>

namespace domain
{

Player::Player(int id, std::string name, Room *r) : Character(name, r), id(id) {
	state = new StatePlayerNormal(this);
	hb_token = clock();
	can_jump = true;
	temporal_name = name;
}

Player::~Player() {
	//TODO: borrar dinamyc memory que faci servir (llistes de write  iskills) i el estat
}

bool Player::is_player() {
	return true;
}

void Player::restore_name() {
	temporal_name = name;
}

std::string Player::get_real_name() {
	return name;
}

void Player::change_state_disconnected() {
	delete state;
	state = new StatePlayerDisconnected(this);
}


bool Player::can_quit() {
	return !is_in_combat();
}

int Player::get_id() {
	return id;
}

clock_t Player::HB_TIME = 3100000;

void Player::hb_action() {
	if (have_hb()) {
		if (is_in_combat()) {
			std::set<Character*>* room_chars = room->get_characters();
			std::multiset<Character*> intersect;
			std::set_intersection(
					room_chars->begin(),
					room_chars->end(),
					in_combat.begin(), in_combat.end(),
					std::inserter(intersect, intersect.begin()));

			if (!intersect.empty()) {
				// Escollir nomes 1 de la interseccio
				std::multiset<Character*>::iterator it = intersect.begin();
				int t = rand()%intersect.size();
				for (int j=0; j<t; ++it, ++j);
				Character *choosen = *it;

				CombatController::get_instance()->injure_mele(this, choosen);
			}
		}
		// A l'hora de desapilar un HBSkill de la pila hb_skills_to_do, abans sempre
		// sa de fer un if (s->end()) delete s;
	}
}


bool Player::have_hb() {
	// Comprovem si pot efectuar una accio de hb en aquest moment
	int aux = (clock() - hb_token) % Player::HB_TIME;
	bool have_hb = (aux < Player::HB_TIME*0.02);

	if (!have_hb && !can_jump) can_jump = true;
	if (have_hb && can_jump) {
		can_jump = false;
		return true;
	}
	return false;
}


void Player::push_command(Skill *s) {
	skills_to_do.push_back(s);
}

void Player::push_hb_command(HBSkill *s) {
	for (int i=0; i<s->get_turns(); ++i) {
		hb_skills_to_do.push_back(s);
	}
	// Fem aqui la primera execucio del comando. No comprovem si es l'ultima perque tot skill en hb te com aminim
	// 2 execucions.
	SkillsController::get_instance()->execute(this, s);
}

void Player::add_skill(int n) {
	skills.push_back(n);
}

bool Player::has_skill(int id) {
	std::list<int>::iterator result = std::find(skills.begin(), skills.end(), id);
	return result != skills.end();
}

/**
 * Parseja tot els comandos que te en aquest moment pendents de ser tractats
 */
void Player::flush_commands() {
	if (skills_to_do.begin() == skills_to_do.end()) return;
	Skill *s = skills_to_do[0];

	SkillsController::get_instance()->execute(this, s);

	delete s;
	skills_to_do.pop_front();
	write(new gmp::GMP_msg(">", true));
	flush_commands();
}


void Player::die() {
	//informa al jugador de la seva mort
	gmp::GMP_msg *msg = new gmp::GMP_msg("Tus tripas se esparcen por ahi. Parece que te han matado..., y tus armas caen al suelo..");
	write(msg);

	Character::die();
	temporal_name = "Espiritu de "+name;
}

std::string Player::get_name() {
	return temporal_name;
}

// *************** FUNCIONS D'ESTAT DEL PLAYER *******************
void Player::write(gmp::GMP_msg *msg) {
	state->write(msg);
}

void Player::send_info() {
	state->send_info();
}
// ***************************************************************







StatePlayer::StatePlayer(Player *p) : this_player(p)
{
}

StatePlayer::~StatePlayer() {
//TODO: recorre les llistes msg_to_write i skills_to_do i eliminar els seus objectes amb delete
}


StatePlayerNormal::StatePlayerNormal(Player *p): StatePlayer(p)
{
}

StatePlayerNormal::~StatePlayerNormal()
{
}


void StatePlayerNormal::write(gmp::GMP_msg *msg) {
	this_player->msg_to_write.push_back(msg);
}



void StatePlayerNormal::send_info() {
	if (this_player->msg_to_write.begin() == this_player->msg_to_write.end()) return;
	gmp::GMP_msg *s = this_player->msg_to_write[0];

	net::ClientManager::get_instance()->send_to_client(this_player->id, s);

	delete s;

	this_player->msg_to_write.pop_front();
	//this_player->msg_to_write.clear();
	send_info();
}


StatePlayerDisconnected::StatePlayerDisconnected(Player *p) : StatePlayer(p) {
	//this_player->set_name("Estatua de "+this_player->get_name());
	this_player->temporal_name = "Estatua de "+this_player->name;
}

StatePlayerDisconnected::~StatePlayerDisconnected() {
}

/** Inhabilitem l'enviament de dades a un player desconectat
 */
void StatePlayerDisconnected::write(gmp::GMP_msg *msg) {

}

void StatePlayerDisconnected::send_info() {

}

}
