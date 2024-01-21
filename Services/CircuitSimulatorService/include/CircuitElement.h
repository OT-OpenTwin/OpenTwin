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
	std::string getUID();
	std::string getValue();
	std::set<Connection>& getList();

	//Setter
	void setItemName(std::string name);
	void setEditorName(std::string name);
	void setUID(std::string id);
	void setValue(std::string name);

	//Additional Functions
	void addConnection(const Connection& obj);
	
	
	

private:
	

	//Attributes
	std::string m_itemName;
	std::string m_editorName;
	std::string Uid;
	std::string value;
	std::set<Connection> listOfConnections;
	

	
};