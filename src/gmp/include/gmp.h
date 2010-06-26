#ifndef GMP_H_
#define GMP_H_

#include <GMP_msg.h>

namespace gmp {

class GMP_Procotol {
protected:
	GMP_Procotol();
	~GMP_Procotol();
	GMP_Procotol(const GMP_Procotol&);
	GMP_Procotol& operator= (const GMP_Procotol&);

private:
	static GMP_Procotol *instance;

public:
	static GMP_Procotol* get_instance();

	void parse(int player_id, const char *buff, int len);

	GMP_msg* get_died_player_error_msg();
};


}

#endif /*GMP_H_*/
