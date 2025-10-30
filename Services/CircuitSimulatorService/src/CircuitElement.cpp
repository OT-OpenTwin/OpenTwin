// @otlicense
// File: CircuitElement.cpp
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
#include "CircuitElement.h"

//C++ Header
#include <string>
#include <algorithm>
#include <vector>

//Construtor
CircuitElement::CircuitElement( std::string itemName,  std::string editorName, ot::UID Uid,  std::string netlistName)
	: m_itemName(itemName), m_editorName(editorName), m_Uid(Uid), m_netlistName(netlistName)
{

}


//Getter

std::string CircuitElement::getItemName()	{return this->m_itemName;}

std::string CircuitElement::getEditorName()   {return this->m_editorName;}

ot::UID CircuitElement::getUID() { return this->m_Uid; }

std::map<std::string,Connection>& CircuitElement::getList() { return m_listOfConnections; }

std::string CircuitElement::getNetlistName() { return this->m_netlistName; }

std::string CircuitElement::getCustomName() { return this->m_customName; }

std::string CircuitElement::getModel() { return this->m_model; }

std::string CircuitElement::getFolderName() { return this->m_folderName; }

//Setter
void CircuitElement::setItemName(std::string name)	{this->m_itemName = name;}

void CircuitElement::setEditorName(std::string name)	{this->m_editorName = name;}

void CircuitElement::setUID(ot::UID id) { this->m_Uid = id; }


void CircuitElement::setNetlistName(std::string name) { this->m_netlistName = name; }

void CircuitElement::setCustomName(std::string name) {	this->m_customName = name; }

void CircuitElement::setModel(std::string _model) { this->m_model = _model; }

void CircuitElement::setFolderName(std::string _folderName) { this->m_folderName = _folderName; }

//Additional Functions

void  CircuitElement::addConnection(std::string connactable,const Connection& obj)  {
	 m_listOfConnections.insert_or_assign(connactable,obj);
	
}


