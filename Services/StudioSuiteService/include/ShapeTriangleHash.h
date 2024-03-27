#pragma once

#include <string>
#include <map>
#include <list>

class Application;

class ShapeTriangleHash
{
public:
	ShapeTriangleHash() : application(nullptr), hasChanged(false) {};
	~ShapeTriangleHash() {};

	void setData(Application* app);
	void readInformation();
	void getShapes(std::map<std::string, bool>& shapes);
	void setShapeHash(const std::string& name, const std::string& hash);
	void deleteShape(const std::string& name);
	void writeInformation();
	void addDeletedShapesToList(std::list<std::string>& list);

private:
	Application* application;
	bool hasChanged;
	std::map<std::string, std::string> shapeHashMap;
	std::list<std::string> deletedShapes;
};
