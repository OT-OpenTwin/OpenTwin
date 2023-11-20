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

//C++ Header
#include <string>
#include <list>
#include <map>

class CircuitElement
{
public:

	CircuitElement();
	~CircuitElement();

	//Getter
	std::string getItemName();
	std::string getEditorName();
	std::string getUID();
	std::string getNetlistElementName();
	std::list<Connection>& getList();

	//Setter
	void setItemName(std::string name);
	void setEditorName(std::string name);
	void setUID(std::string id);
	void setNetlistElementName(std::string name);

	//Additional Functions
	void addConnection(const Connection& obj);
	bool compareOfNodeNumbers( Connection& objA,  Connection& objB);
	void sortListOfConnections(std::list<Connection>& listOfConnections);
	
	

private:
	

	//Attributes
	std::string m_itemName;
	std::string m_editorName;
	std::string Uid;
	std::string netlistElementName;
	std::list<Connection> listOfConnections;
	

	
};