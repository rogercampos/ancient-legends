#ifndef DAMAGECONTROLLER_H_
#define DAMAGECONTROLLER_H_

#include <character.h>

namespace domain {

class CombatController
{
protected:
	CombatController();
	~CombatController();
	CombatController(const CombatController&);
	CombatController& operator= (const CombatController&);

private:
	static CombatController *instance;
	void injure(Character *from, Character *to,  int);

public:
	static CombatController* get_instance();

    void injure_mele(Character *from, Character *to);
};





}

#endif /*DAMAGECONTROLLER_H_*/
