#include <zone.h>

namespace domain
{

Zone::Zone(int id, std::string name, std::string desc)
{
	this->id = id;
	this->name = name;
	this->desc = desc;
}

Zone::~Zone()
{
}

}
