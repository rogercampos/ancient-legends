#ifndef ZONE_H_
#define ZONE_H_

#include <string>

namespace domain
{

class Zone
{
private:
	int id;
	std::string name;
	std::string desc;

public:
	Zone(int id, std::string name, std::string desc);
	virtual ~Zone();

	std::string get_name() {
		return name;
	}

	std::string get_desc() {
		return desc;
	}
};

}

#endif /*ZONE_H_*/
