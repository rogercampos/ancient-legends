#include <combatController.h>
#include <worldController.h>
#include <GMP_msg.h>
#include <player.h>
#include <stdlib.h>

namespace domain {



CombatController* CombatController::instance = 0;

CombatController* CombatController::get_instance() {
	if (instance == 0) {
		instance = new CombatController;
	}
	return instance;
}

CombatController::CombatController()
{
}

CombatController::~CombatController()
{
}


void CombatController::injure_mele(Character *from, Character *to) {
	//Fa mal de from a to
	/*- 1er.- aconsegueix el mal que fa el atacan
	 *	aqui s'hauria de consultar l'arma que porta from, els seus stats, etc.. per
	 * trobar el seu mal de mele. Tb aqui aniria calcul de critics, pifies, etc...
	 * nomes per les de mele, esclar.
	 */
	int mal = 10 +(rand()%10);

	/* 2n.- Aconsegueix el mal que rep la victima
	 * Aqui es consulta a la funcio get_damage_injured, que s'encarrega de trobar
	 * el mal que realment ha de rebre el objectiu.
	 * Aquesta funcio te en compte l'armadura, resist. magiques, stats, etc de la victima.
	 * Se li pasen amb "params" totes les variables que ha de tenir en compte respecte el mla
	 * que esta rebent la victima, que defineixen aquell tipus de mal, ex:
	 * - TYPE		fisic	tallant
	 * 						perforant
	 * 						aplastant
	 * 				magic	natura
	 * 						fred
	 * 						foc
	 * 						etc....
	 *
	 * - traspasa larmadura del contrari un 30% extra
	 * - ignora un 20% de resist magiques del enemic
	 * - fa un 10% mes de mal contra elfos
	 */
	mal = mal*0.9; // get_damage_injured(mal, to, "params");


	// 3a .- aconsegueix les frases del mal segons larma
	if (from->is_player()) {
		gmp::GMP_msg *msg = new gmp::GMP_msg("# Infliges ");
		msg->append(mal)->append(" de daño a "+to->get_name());
		((Player*)from)->write(msg);
	}

	if (to->is_player()) {
		gmp::GMP_msg *msg2 = new gmp::GMP_msg("* "+from->get_name()+" te hace ");
		msg2->append(mal)->append(" de daño");
		((Player*)to)->write(msg2);
	}

	gmp::GMP_msg msg3(from->get_name()+" hace daño a "+to->get_name());
	WorldController::get_instance()->broadcast(from->get_room(), msg3, from, to);

	// 3.- aplica el mal
	injure(from, to, mal);
}

/** Aplica el mal donat al Character p
 * El mal donat es inflinjit "as is" a p, es a dir aquesta es una funcio totalment generica.
 * La comprovacio de reduccions de mal, etc.. s'efectuen a nivells superiors.
 * -> *from pot ser =NULL si l'origen del mal no es un Character, (acid del terra, caigudes, etc..)
 * -> Aqui es controla la condicio de "mort" de la victima p
 */
void CombatController::injure(Character *from, Character *p, int mal) {
	  p->set_damage(mal);

	  if (!p->is_alive()) {
		  //Atura la lluita amb p dels altres players que estiguesin en combat amb ell
		  std::set<Character*>* list = p->get_in_combat();
		  for (std::set<Character*>::iterator i = list->begin();
			  			i != list->end(); ++i) {
				  (*i)->remove_combat(p);
		  }

		  //Informa al player from que ha mort a l'enemic
		  if (from != NULL) {
			  if (from->is_player()) {
				  gmp::GMP_msg *msg = new gmp::GMP_msg("Propinas el golpe mortal a "+p->get_name());
			  	  ((Player*)from)->write(msg);
			  }
		  }

		  gmp::GMP_msg tp(from->get_name()+" propina el golpe mortal a "+p->get_name());
		  WorldController::get_instance()->broadcast(p->get_room(), tp, from);

		  //Cambia d'estat el player p a mort
		  p->die();
	  }
	  if (p->is_player()) {
		  gmp::GMP_msg *msg= new gmp::GMP_msg();
		  msg->append("PV: ")->append(p->get_pvs())->append("/")->append(p->get_max_pvs());
		  ((Player*)p)->write(msg);
	  }
}





}