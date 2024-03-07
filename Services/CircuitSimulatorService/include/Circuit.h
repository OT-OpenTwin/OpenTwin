#pragma once

//Service Header
#include "CircuitElement.h"

//C++ Header
#include <map>



class Circuit
{
public:
	Circuit();
	~Circuit();

	
	std::string getEditorName();
	std::string getId();
	std::map<ot::UID, CircuitElement>& getMapOfElements();
	void addElement(ot::UID key, const CircuitElement& obj);
	void setEditorName(std::string name);
	void setId(const std::string id);
	
	//additionalFunctions
	bool addConnection(const ot::UID& key, const Connection& obj);
	std::string findElement(const ot::UID& key);
		
private:

	std::map <ot::UID, CircuitElement> mapOfElements;
	std::string editorName;
	std::string id;
	

};