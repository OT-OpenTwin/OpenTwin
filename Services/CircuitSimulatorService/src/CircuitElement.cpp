/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

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

std::set<Connection>& CircuitElement::getList() { return m_listOfConnections; }

std::string CircuitElement::getNetlistName() { return this->m_netlistName; }

//Setter
void CircuitElement::setItemName(std::string name)	{this->m_itemName = name;}

void CircuitElement::setEditorName(std::string name)	{this->m_editorName = name;}

void CircuitElement::setUID(ot::UID id) { this->m_Uid = id; }


void CircuitElement::setNetlistName(std::string name) { this->m_netlistName = name; }

//Additional Functions

bool CircuitElement::addConnection(const Connection& obj)  {
	auto result = m_listOfConnections.insert(obj);
	return result.second;
}


