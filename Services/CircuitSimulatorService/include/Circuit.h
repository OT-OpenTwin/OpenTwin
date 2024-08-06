#pragma once

//Service Header
#include "CircuitElement.h"
#include "EntityBlock.h"
//C++ Header
#include <map>



class Circuit
{
public:

	// Existing move constructor
	Circuit(Circuit&& other) noexcept = default;

	// Copy constructor
	Circuit(const Circuit& other) = default;

	// Copy assignment operator
	Circuit& operator=(const Circuit& other) = default;

	// Move assignment operator
	Circuit& operator=(Circuit&& other) noexcept = default;


	Circuit();
	~Circuit();

	//Getter
	std::string getEditorName();
	std::string getId();
	std::map<ot::UID, CircuitElement*>& getMapOfElements();
	std::map <std::string, std::vector<std::shared_ptr<EntityBlock>>>& getMapOfEntityBlcks();

	//Setter
	void addElement(ot::UID key, CircuitElement* obj);
	void addBlockEntity(std::string block, const std::shared_ptr<EntityBlock> obj);
	void setEditorName(std::string name);
	void setId(const std::string id);
	
	//additionalFunctions
/*	bool*/ void addConnection(const ot::UID& key, const Connection& obj);
	std::string findElement(const ot::UID& key);
		
private:

	std::map <ot::UID, CircuitElement*> mapOfElements;
	std::map <std::string, std::vector<std::shared_ptr<EntityBlock>>> mapOfEntityBlocks;
	std::string editorName;
	std::string id;
	

};