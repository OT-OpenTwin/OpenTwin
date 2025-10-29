// @otlicense

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

void Circuit::addConnection(std::string connactable,const ot::UID& key, const Connection& obj) {

	//Here we extrac the element out of the connectorName to check wheter its a connector or not 
	std::string element = Application::instance()->extractStringAfterDelimiter(connactable, '/', 2);
	element = element.substr(0, element.find('_'));


	if (element == "Connector") {
		//The Connector element we ignore because it does not belong to the CircuitElements so it does not need any connection 
		return;
	}

	if (mapOfElements.find(key) != mapOfElements.end()) {
		mapOfElements[key]->addConnection(connactable,obj);
	

	}
	else if (connactable != "GNDPole") {
		Application::instance()->getUiComponent()->displayMessage("Element not found"); // Auf OtLog umändern
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
