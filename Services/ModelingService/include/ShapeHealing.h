// @otlicense

#pragma once

#include <string>

class Application;
class EntityCache;

class TopoDS_Shape;

class ShapeHealing
{
public:
	ShapeHealing(Application *app) : application(app) {};
	virtual ~ShapeHealing() {};

	void healSelectedShapes(double tolerance, bool fixSmallEdges, bool fixSmallFaces, bool sewFaces, bool makeSolids);
	void healShape(const std::string &name, TopoDS_Shape &myshape, double tolerance, bool fixSmallEdges, bool fixSmallFaces, bool sewFaces, bool makeSolids, std::string &messages);

private:
	Application *application;
};