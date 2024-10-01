#include "Polygon.h"

MyPolygon::MyPolygon(unsigned int l, std::vector<std::pair<int, int>> c) : layer(l), coordinates(c){}

MyPolygon::MyPolygon() {
	layer = 0;
	coordinates = {};
}

void MyPolygon::setCoordinates(std::vector<std::pair<int, int>> coords){ coordinates = coords; }

void MyPolygon::setLayer(unsigned int layer) { this->layer = layer; }

unsigned int MyPolygon::getLayer() { return layer; }

std::vector<std::pair<int, int>> MyPolygon::getCoordinates() { return coordinates; }
