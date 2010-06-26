#include <iostream>
#include <stdlib.h>
#include <errno.h>

#include <mud_error.h>


void mud_error(const char *s) {
	std::cerr << s << std::endl;
	exit(-1);
}

void mud_perror(const char *s) {
	perror(s);
	exit(-1);
}
