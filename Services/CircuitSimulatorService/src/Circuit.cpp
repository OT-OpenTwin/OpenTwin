// @otlicense
// File: Circuit.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
		Application::instance()->getUiComponent()->displayMessage("Element not found");
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
