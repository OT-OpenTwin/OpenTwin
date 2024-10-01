#include "Layer.h"

Layer::Layer(int id_, std::vector<MyPolygon> poly, float height_, float thickness_)
	: id(id_), polygons(poly), height(height_), thikness(thickness_), paths({}) {}

int Layer::getId() { return id; }

float Layer::getThikness() { return thikness; }

float Layer::getHeight() { return height; }

std::vector<MyPolygon> Layer::getPolygons() { return polygons; }

std::vector<Gds2Path> Layer::getPaths(){return paths;}

void Layer::setPaths(std::vector<Gds2Path> _paths) { paths = _paths; }
