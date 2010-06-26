#include <iostream>
#include <sstream>

#include <skillsActions.h>
#include <GMP_msg.h>
#include <room.h>
#include <zone.h>
#include <worldController.h>
#include <playersController.h>
#include <types.h>
#include <character.h>
#include <gmp.h>

namespace domain {

SkillsActions* SkillsActions::instance = 0;

SkillsActions* SkillsActions::get_instance() {
  if (instance == 0) {
    instance = new SkillsActions;
  }
  return instance;
}

SkillsActions::SkillsActions()
{
}
SkillsActions::~SkillsActions()
{
}

/**			 CHARACTER SKILLS		************************************
 *
 * ************************************************************************/

  void SkillsActions::n(Player *p, Skill *s) {
    move_wrapper(p, "n");
  }

  void SkillsActions::ne(Player *p, Skill *s) {
    move_wrapper(p, "ne");
  }
  void SkillsActions::e(Player *p, Skill *s) {
    move_wrapper(p, "e");
  }
  void SkillsActions::se(Player *p, Skill *s) {
    move_wrapper(p, "se");
  }
  void SkillsActions::s(Player *p, Skill *s) {
    move_wrapper(p, "s");
  }
  void SkillsActions::so(Player *p, Skill *s) {
    move_wrapper(p, "so");
  }
  void SkillsActions::o(Player *p, Skill *s) {
    move_wrapper(p, "o");
  }
  void SkillsActions::no(Player *p, Skill *s) {
    move_wrapper(p, "no");
  }

  void SkillsActions::decir(Player *p, Skill *s) {
    gmp::GMP_msg msg(p->get_name()+" dice: "+s->get_param());
    WorldController::get_instance()->broadcast(p->get_room(), msg, p);
    gmp::GMP_msg *msg2 = new gmp::GMP_msg("Dices: "+s->get_param());
    p->write(msg2);
  }

  void SkillsActions::matar(Player *p, Skill *s) {
	  if (!p->is_alive()) {
		  p->write(gmp::GMP_Procotol::get_instance()->get_died_player_error_msg());
		  return;
	  }
	  Character *c = p->get_room()->get_character_by_name(s->get_param());
	  if (c == NULL) {
		  gmp::GMP_msg *msg = new gmp::GMP_msg("No encuentro a "+s->get_param());
		  p->write(msg);
	  }
	  else if (c == p) {
		  gmp::GMP_msg *msg = new gmp::GMP_msg("Intentas suicidarte!?");
		  p->write(msg);
	  }
	  else if (!c->is_alive()) {
		  gmp::GMP_msg *msg = new gmp::GMP_msg("Ya esta muerto, dejalo que bastante tiene ya con lo suyo.");
		  p->write(msg);
	  }
	  else {
		  gmp::GMP_msg *msg = new gmp::GMP_msg("Ok. La lucha con "+s->get_param()+" comienza");
		  p->write(msg);
		  p->add_combat(c);
		  c->add_combat(p);
		  if (c->is_player()) {
			  gmp::GMP_msg *msg2 = new gmp::GMP_msg("Estas siendo atacado por "+p->get_name());
			  ((Player*)c)->write(msg2);
		  }
	  }
  }

/**			 PLAYER SKILLS		************************************
*
* ************************************************************************/

  void SkillsActions::glance(Player *p, Skill *s) {
    std::string txt;
    domain::Room *r = p->get_room();
    domain::Zone *z = r->get_zone();
    int v = r->valid_exits();
    std::string res = "[";
    if (v & 128) {
      res += "n, ";
    }
    if (v & 64) {
      res += "ne, ";
    }
    if (v & 32) {
      res += "e, ";
    }
    if (v & 16) {
      res += "se, ";
    }
    if (v & 8) {
      res += "s, ";
    }
    if (v & 4) {
      res += "so, ";
    }
    if (v & 2) {
      res += "o, ";
    }
    if (v & 1) {
      res += "no";
    }
    res+="]";
    txt = "Lloc: " + z->get_name() + " - " + r->get_name();
    txt += "\nDesc: " + z->get_desc() + " - " + r->get_desc();
    txt += "\nSortides: " + res;
    txt += "\nPlayers: ";
    std::set<Character*>* players = r->get_characters();
	for (std::set<Character*>::iterator i = players->begin();
			i != players->end(); ++i) {
		if ((*i)->get_name() != p->get_name()) txt += (*i)->get_name() + " ";
	}

    gmp::GMP_msg *msg= new gmp::GMP_msg(txt);
    p->write(msg);
  }

  void SkillsActions::sc(Player *p, Skill *s) {
	  gmp::GMP_msg *msg= new gmp::GMP_msg();
	  msg->append("PV: ")->append(p->get_pvs())->append("/")->append(p->get_max_pvs());
	  p->write(msg);
  }

  void SkillsActions::tell(Player *p, Skill *s) {
	  std::string param = s->get_param();
	  int token = param.find(" ");
	  std::string target = param.substr(0, token);
	  Player *to = PlayersController::get_instance()->get_player(target);
	  if (to ==NULL) {
		  gmp::GMP_msg *msg= new gmp::GMP_msg("No existe la persona.");
		  p->write(msg);
	  }
	  else {
		  std::string bar = param.substr(token+1);
		  gmp::GMP_msg *msg= new gmp::GMP_msg(p->get_name()+" te dice: "+bar);
		  to->write(msg);
		  gmp::GMP_msg *msg2= new gmp::GMP_msg("Dices a "+to->get_name()+": "+bar);
		  p->write(msg2);
	  }
  }


  void SkillsActions::salir(Player *p, Skill *s) {
	  if (p->can_quit()) {
		  PlayersController::get_instance()->remove_player(p);
	  }
	  else {
		  gmp::GMP_msg *msg= new gmp::GMP_msg("Termina el combate antes de salir.");
		  p->write(msg);
	  }
  }

  void SkillsActions::ficha(Player *p, Skill *s) {
	  gmp::GMP_msg *msg = new gmp::GMP_msg("Nombre: "+p->get_name());
	  p->write(msg);
  }

  void SkillsActions::peleas(Player *p, Skill *s) {
	  std::set<Character*>* list = p->get_in_combat();
	  if (list->empty()) {
		  gmp::GMP_msg *msg = new gmp::GMP_msg("No estas peleando con nadie en este momento");
		  p->write(msg);
	  }
	  else {
		  std::string txt = "Estas peleando con: ";
		  for (std::set<Character*>::iterator i = list->begin();
		  			i != list->end(); ++i) {
			  txt+= (*i)->get_name()+", ";
		  }
		  gmp::GMP_msg *msg = new gmp::GMP_msg(txt);
		  p->write(msg);
	  }
  }

  void SkillsActions::stoppeleas(Player *p, Skill *s) {

  }




/**			 GUILD SKILLS		************************************
*
* ************************************************************************/

  void SkillsActions::gcritico(Player *p, Skill *s) {

  }


  void SkillsActions::formular(Player *p, Skill *s) {
	  //enviar resposta instnat als clients (X comienza a formular)

	  //afegir a p un HBAction que representi el hechizo
  }

  void SkillsActions::golpear(Player *p, Skill *s) {
	  int turn = ((HBSkill*)s)->get_turn();
	  if (turn == 0) {
		  // Buscas un punto debil en la defensa de tu enemigo cogiendo carrerilla.
	  }
	  else if (turn == 1) {
		  // Asestas un terrible golpe a tu enemigo.
		  /*
		   * -> Demanes a la clase del teu guild que et torni el mal que has fet amb el golpeo.
		   * (aqui es calcula la teva força, la teva arma, els teus buffos, etc...)
		   *
		   * -> Demanes a la clase DamageController que adjudiqui aquet mal en el enemic, amb els flags
		   * que toquin
		   * (potser aquet atac te un 50% mes de critic, o un 20% de stun al enemic, o
		   * ignora 40% de armadura del enemic, etc...)
		   * (aqui es calcula el mal que rep l'enemic segons el seu estat. Es tenen en compte
		   * buffos / debuffos del enemic, la seva armadura, les seves resist. magiques, si esta en
		   * un estat de "inmune a tot", etc...)
		   */
	  }
	  ((HBSkill*)s)->pass_hb();
  }



/**			 AUX FUNCTIONS		************************************
*
*************************************************************************/



  void SkillsActions::move_player(Player *p, Room *from, Room *dest) {
    from->remove_character(p);
    dest->add_character(p);
    p->set_room(dest);
  }

  void SkillsActions::move_wrapper(Player *p, std::string e) {
    Room *r = p->get_room();
    Direction dir = Direction::str2card(e);
    if (r->has_exit(dir)) {
		gmp::GMP_msg msg(p->get_name()+" se va hacia "+dir.get_goto_description());
		WorldController::get_instance()->broadcast(r, msg, p);
	    move_player(p, r, r->get_exit(dir));
		gmp::GMP_msg msg2(p->get_name()+" llega "+dir.reverse().get_from_description());
		WorldController::get_instance()->broadcast(p->get_room(), msg2, p);
	    glance(p, NULL);

    }
    else {
      //TODO: generar gmp_error i tornarlo
    }
  }

}
