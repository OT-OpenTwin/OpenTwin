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
#include "OTCore/Geometry/Point2D.h"
#include "OTBlockEntities/EntityBlock.h"
#include "OTCore/InClassFactory.h"
#include "OTCore/OTClassHelper.h"

//C++ Header
#include <string>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <memory>

class CircuitElement
{
	OT_DECL_NOCOPY(CircuitElement)
	OT_DECL_INCLASS_FACTORY(CircuitElement, CircuitElement, Registrar)
public:
	CircuitElement() = default;
	CircuitElement(std::string itemName,  std::string editorName, ot::UID Uid,  std::string netlistName);
	virtual ~CircuitElement() {}
	virtual std::string type() const = 0;

	// Factory method to create a CircuitElement from an EntityBlock
	static CircuitElement* createFromClassName(const std::string& _className);

	//Getter
	std::string getItemName();
	std::string getEditorName();
	ot::UID getUID();
	std::map<std::string ,Connection>& getList();
	std::string getNetlistName();
	std::string getCustomName();
	std::string getModel() const;
	std::string getFolderName();

	//Setter
	void setItemName(std::string name);
	void setEditorName(std::string name);
	void setUID(ot::UID id);
	void setNetlistName(std::string name);
	void setCustomName(std::string name);
	void setModel(std::string _model);
	void setFolderName(std::string _folderName);
	void addConnection(std::string connactable,const Connection& obj);

	virtual std::string getNetlistPrefix() const = 0;
	virtual std::string getNetlistValue() const = 0;
	virtual bool isMeter() const { return false; };
	virtual bool isTransmissionLine() const { return false; };

	virtual std::vector<std::string> getPositivePoleNames() const { return { "positivePole" }; };
	virtual std::vector<std::string> getNegativePoleNames() const { return { "negativePole" }; };
	
	virtual void initFromEntity(const std::shared_ptr<ot::EntityBlock>& _entity, const std::string& _editorName) = 0;
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
};