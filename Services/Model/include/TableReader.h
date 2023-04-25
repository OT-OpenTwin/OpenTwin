#pragma once

#include <string>
#include <list>

class Model;
class EntityObserver;
class ModelState;
class ClassFactory;

class TableReader
{
public:
	TableReader();
	virtual ~TableReader();

	void setModel(Model *mdl) { model = mdl; }

	std::string readFromFile(const std::string fileName, const std::string &itemName, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);

private:
	bool readNextLine(std::ifstream &file, std::string &line);
	std::list<std::string> separateLine(std::string &line, char separator);
	void trimString(std::string &str);

	Model *model;
};
