#ifndef SKILLSACTIONS_H_
#define SKILLSACTIONS_H_

#include <player.h>
#include <skill.h>

namespace domain {


class SkillsActions {
protected:
	SkillsActions();
	~SkillsActions();
	SkillsActions(const SkillsActions&);
	SkillsActions& operator= (const SkillsActions&);

private:
	static SkillsActions *instance;

	void move_player(Player *p, Room *from, Room *dest);
	void move_wrapper(Player *p, std::string e);


public:
	static SkillsActions* get_instance();

/**			 CHARACTER SKILLS		************************************
 *
 * ************************************************************************/

	void n(Player *p, Skill *s);
	void ne(Player *p, Skill *s);
	void e(Player *p, Skill *s);
	void se(Player *p, Skill *s);
	void s(Player *p, Skill *s);
	void so(Player *p, Skill *s);
	void o(Player *p, Skill *s);
	void no(Player *p, Skill *s);

	void decir(Player *p, Skill *s);
	void matar(Player *p, Skill *s);

/**			 PLAYER SKILLS		************************************
*
* ************************************************************************/

	void glance(Player *p, Skill *s);
	void sc(Player *p, Skill *s);
	void tell(Player *p, Skill *s);
	void salir(Player *p, Skill *s);
	void ficha(Player *p, Skill *s);
	void peleas(Player *p, Skill *s);
	void stoppeleas(Player *p, Skill *s);


/**			 GUILD SKILLS		************************************
*
* ************************************************************************/

	void gcritico(Player *p, Skill *s);
	void formular(Player *p, Skill *s);
	void golpear(Player *p, Skill *s);

};

}
#endif /*SKILLSACTIONS_H_*/
