#ifndef GMP_MSG_H_
#define GMP_MSG_H_

#include <string>

namespace gmp {

class GMP_msg {
private:
	std::string msg;
	bool skip;

public:
	GMP_msg(GMP_msg *in);
	GMP_msg(std::string msg="", bool skip = false);
	virtual ~GMP_msg();

	const char *get_msg();
	int get_len();
	GMP_msg* append(int);
	GMP_msg* append(std::string);
	GMP_msg *clone();
	void finalize();
};

}

#endif /*GMP_MSG_H_*/
