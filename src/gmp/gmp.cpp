#include <string>
#include <iostream>


#include <gmp.h>
#include <skillsController.h>
#include <skill.h>
#include <playersController.h>
#include <player.h>

namespace gmp {

GMP_Procotol* GMP_Procotol::instance = 0;

GMP_Procotol* GMP_Procotol::get_instance() {
	if (instance == 0) {
		instance = new GMP_Procotol;
	}
	return instance;
}

GMP_Procotol::GMP_Procotol()
{
}

GMP_Procotol::~GMP_Procotol()
{
}

/** Parseja un missatge enviat per un client. FIXME: OJO !! Aquesta funcio
 * es especifica per tractar info rebuda per telnet
 * - Verifica que ens hagui enviat un comando valid.
 * - si es valid, ens crea l'instancia del skill i el puja a la pila de l'usuari
 * - si no es valid torna error al client
 */
void GMP_Procotol::parse(int player_id, const char *buff, int len) {
/** TODO: Faltaria afegir un control de keep Alive a la conexio.
 * Cada X segons el server envia un misatge especia lal client, i aquest
 * respon amb un misatge tb especific.
 * Faltaria verificar aqui si el paquet llegit es un keepAliveACK
 *
 */
	std::string s, name, param;
	s = std::string(buff, len);

	//FIXME: Cambiar aixo per un trim()
	s = s.substr(0, s.length()-2);

	size_t i = s.find_first_of(" ");
	if (i == std::string::npos) {
		name = s;
	}
	else {
		name = s.substr(0, i);
		param = s.substr(i+1);
	}

	domain::Skill *sk = domain::SkillsController::get_instance()->get_skill_by_name(name);
	if (sk == NULL) {
		// El que ens han pasat no es cap comando valid
		// GMP_msg msg = get_err_no_skill(); TODO
		//enviem el msg al client
		std::cerr << "Comando no valid [" << name << "]" <<std::endl;
		return;
	}

	// Afegim la resta del comando com a parametre del skill
	sk->set_param(param);

	domain::Player *p = domain::PlayersController::get_instance()->get_player(player_id);
	if (p->has_skill(sk->get_id())) {
		p->push_command(sk);
		//domain::SkillsController::get_instance()->add_skill_to_player(p, sk);
	}
	else {
		// El player ens ha enviat un skill que existeix, pero que encara no té
		// Generem GMP_msg d'error i tornali TODO
		std::cerr << "El jugador no te el comando" << std::endl;
	}
}


GMP_msg* GMP_Procotol::get_died_player_error_msg() {
	return new GMP_msg("No puedes realizar esa accion mientras estas en tu forma espiritual.");
}


}
