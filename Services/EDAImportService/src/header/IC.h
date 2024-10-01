#ifndef IC_H_
#define IC_H_

#include <vector>
#include <string>
#include "Layer.h"

class IC{
private:
	std::string name; // name of the ic
	std::vector<Layer> layers; // layer of which the ic is build up

public:
	IC(std::string name_, std::vector<Layer> layers_);

	std::string getName();
	std::vector<Layer> getLayers();

};

#endif

