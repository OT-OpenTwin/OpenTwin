// @otlicense
// File: CircuitElement.h
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
#include "Connection.h"

#pragma once
//OT Header
#include "OTCore/Point2D.h"

//C++ Header
#include <string>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <list>

class CircuitElement
{


protected:


	//Attributes
	std::string  m_itemName;
	std::string m_editorName;
	ot::UID m_Uid;
	std::string m_netlistName;
	std::string m_customName;
	std::map < std::string, Connection > m_listOfConnections;
	std::string m_model;
	std::string m_folderName;
	
public:


	CircuitElement(std::string itemName,  std::string editorName, ot::UID Uid,  std::string netlistName);

	virtual ~CircuitElement() {}

	virtual std::string type() const = 0;

	

	//Getter
	std::string getItemName();
	std::string getEditorName();
	ot::UID getUID();
	std::map<std::string ,Connection>& getList();
	std::string getNetlistName();
	std::string getCustomName();
	std::string getModel();
	std::string getFolderName();

	//Setter
	void setItemName(std::string name);
	void setEditorName(std::string name);
	void setUID(ot::UID id);
	void setNetlistName(std::string name);
	void setCustomName(std::string name);
	void setModel(std::string _model);
	void setFolderName(std::string _folderName);

	//Additional Functions
	/*bool*/ void addConnection(std::string connactable,const Connection& obj);
	
	
	

	

	
};