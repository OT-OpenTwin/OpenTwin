#pragma once
//
////Service Header
#include "CircuitElement.h"

//C++ Header
#include <map>



class Circuit
{
public:
	Circuit();
	~Circuit();

	
	std::string getEditorName();
	void addConnection(const std::string& key,const Connection& obj);
	/*void sort(const std::string& key)*/;
	std::string getId();
	std::map<std::string, CircuitElement> getMapOfElements();
	void addElement(std::string key, const CircuitElement& obj);
	void setEditorName(std::string name);
	void setId(const std::string id);
	
		
private:

	std::map <std::string, CircuitElement> mapOfElements;
	std::string editorName;
	std::string id;
	

};