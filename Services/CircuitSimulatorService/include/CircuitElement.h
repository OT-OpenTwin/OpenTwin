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

class CircuitElement
{
public:

	CircuitElement();
	~CircuitElement();

	//Getter
	std::string getItemName();
	std::string getEditorName();
	ot::UID getUID();
	std::string getValue();
	std::set<Connection>& getList();
	std::string getType();

	//Setter
	void setItemName(std::string name);
	void setEditorName(std::string name);
	void setUID(ot::UID id);
	void setValue(std::string name);
	void setType(std::string type);

	//Additional Functions
	bool addConnection(const Connection& obj);
	
	
	

private:
	

	//Attributes
	std::string m_itemName;
	std::string m_editorName;
	ot::UID Uid;
	std::string value;
	std::string type;
	std::set<Connection> listOfConnections;
	

	
};