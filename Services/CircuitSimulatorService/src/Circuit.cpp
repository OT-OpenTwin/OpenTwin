//Service Header
#include "Circuit.h"
#include "Application.h" 
#include "OTServiceFoundation/UiComponent.h"

//C++ Header
#include <string>


//Constructor
Circuit::Circuit()
{
	
}

Circuit::~Circuit() 
{
	/*for (auto elements : mapOfElements) {
		delete elements.second;
		elements.second = nullptr;
	}*/
}



std::string Circuit::getEditorName() {
	return this->editorName;
}



 std::string Circuit::getId() {
	return this->id;
}

std::map<ot::UID, CircuitElement*>& Circuit::getMapOfElements() {
	return this->mapOfElements;
}

std::map<std::string,std::vector<std::shared_ptr<EntityBlock>>>& Circuit::getMapOfEntityBlcks() {
	return this->mapOfEntityBlocks;
}

void Circuit::addElement(ot::UID key, CircuitElement* obj) {
	mapOfElements.emplace(key,obj);
}

void Circuit::addBlockEntity(std::string block, const std::shared_ptr<EntityBlock> obj) {
	mapOfEntityBlocks[block].push_back(obj);
}



void Circuit::setEditorName(const std::string name) {
	this->editorName = name;
}

void Circuit::setId(std::string id) {
	this->id = id;
}

/*bool*/  void Circuit::addConnection(std::string connactable,const ot::UID& key, const Connection& obj) {
	if (mapOfElements.find(key) != mapOfElements.end()) {
		/*bool result =*/ mapOfElements[key]->addConnection(connactable,obj);
		/*return result;*/

	}
	else {
		Application::instance()->uiComponent()->displayMessage("Element not found"); // Auf OtLog umändern
		/*return false;*/

	}

}

std::string Circuit::findElement(const ot::UID& key) {
	if (mapOfElements.find(key) != mapOfElements.end()) {
		return mapOfElements[key]->getItemName();
	}
	else {
		return "Not Found!";
	}
}
