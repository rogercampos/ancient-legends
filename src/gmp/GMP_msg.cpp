#include <GMP_msg.h>
#include <sstream>


namespace gmp {

/** TODO: Implementar l¡operador << a aquesta clase, perque es pugui
 * utilitzar per exemple :
 * msg << "PVS: " << 10 << "/" << total;
 */
GMP_msg::~GMP_msg() {
}

GMP_msg::GMP_msg(GMP_msg *in) {
	this->msg = in->msg;
	this->skip = in->skip;
}

GMP_msg::GMP_msg(std::string msg, bool skip) {
	this->msg = msg;
	this->skip = skip;
}

const char *GMP_msg::get_msg() {
	return msg.c_str();
}

int GMP_msg::get_len() {
	return msg.length();
}

GMP_msg *GMP_msg::clone() {
	return new GMP_msg(this);
}


GMP_msg *GMP_msg::append(int a) {
	std::stringstream out;
	out << a;
	msg += out.str();
	return this;
}

GMP_msg *GMP_msg::append(std::string a) {
	msg += a;
	return this;
}

void GMP_msg::finalize() {
	if (!skip) msg+="\n";
	msg += "\0";
}

}
