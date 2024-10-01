#include "StructRef.h"

StructRef::StructRef(std::string name, std::pair<int, int> coords) : name(name), coordinates(coords) {}

std::string StructRef::getName() { return name; }

std::pair<int, int> StructRef::getCoordinates() { return coordinates; }

void StructRef::setName(std::string name) {	this->name = name; }

void StructRef::setCoordinates(std::pair<int, int> coords) { this->coordinates = coords; }
