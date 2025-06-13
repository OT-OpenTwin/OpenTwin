/*
 * Application.h
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

//Service Header
#include "Connection.h"

#pragma once
//OT Header
#include "OTCore/Point2D.h"
#include "ClassFactoryBlock.h"

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

	//Setter
	void setItemName(std::string name);
	void setEditorName(std::string name);
	void setUID(ot::UID id);
	void setNetlistName(std::string name);
	void setCustomName(std::string name);
	void setModel(std::string _model);

	//Additional Functions
	/*bool*/ void addConnection(std::string connactable,const Connection& obj);
	
	
	

	

	
};