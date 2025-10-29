// @otlicense

#include "IC.h"

IC::IC(std::string name_, std::vector<Layer> layers_) : name(name_), layers(layers_){}

std::string IC::getName(){ return name; }

std::vector<Layer> IC::getLayers(){	return layers;}
