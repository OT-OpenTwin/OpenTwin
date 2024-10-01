#include "Gds2Structure.h"

Gds2Structure::Gds2Structure(std::string name, std::vector<MyPolygon> polygons) : name(name), polygons(polygons), structureReferences({}) {}

Gds2Structure::Gds2Structure(std::string name) : name(name), polygons({}), structureReferences({}) {}

Gds2Structure::Gds2Structure(): name(""), polygons({}), structureReferences({}) {}

std::string Gds2Structure::getName(){ return name; }

std::vector<MyPolygon> Gds2Structure::getPolygons(){ return polygons; }

std::vector<StructRef> Gds2Structure::getStructRef(){ return structureReferences; }

std::vector<Gds2Path> Gds2Structure::getPaths(){ return paths; }

void Gds2Structure::setStructRef(std::vector<StructRef> structRef) { structureReferences = structRef; }

void Gds2Structure::setPolygons(std::vector<MyPolygon> polygons) { this->polygons = polygons; }

void Gds2Structure::setPaths(std::vector<Gds2Path> paths) { this->paths = paths; }

void Gds2Structure::addStuctRef(StructRef newStrRef){
	structureReferences.push_back(newStrRef);
}
