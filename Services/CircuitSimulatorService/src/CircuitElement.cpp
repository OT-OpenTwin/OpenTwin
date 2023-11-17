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

//Constructor
CircuitElement::CircuitElement()
{
	
}

//Destructor
CircuitElement::~CircuitElement()
{

}

//Getter

std::string CircuitElement::getItemName()	{return this->m_itemName;}

std::string CircuitElement::getEditorName()   {return this->m_editorName;}

std::string CircuitElement::getUID() { return this->Uid; }

std::string CircuitElement::getNetlistElementName() { return this->netlistElementName; }

std::list<Connection>& CircuitElement::getList() { return listOfConnections; }

//Setter
void CircuitElement::setItemName(std::string name)	{this->m_itemName = name;}

void CircuitElement::setEditorName(std::string name)	{this->m_editorName = name;}

void CircuitElement::setUID(std::string id) { this->Uid = id; }

void CircuitElement::setNetlistElementName(std::string name) { this->netlistElementName = name; }

//Additional Functions

void CircuitElement::addConnection(const Connection &obj) { listOfConnections.push_back(obj);}

//bool CircuitElement::compareOfNodeNumbers( Connection& objA,  Connection& objB) 
//{
//	if (std::stoi(objA.getNodeNumber()) < std::stoi(objB.getNodeNumber()))
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}

//void CircuitElement::sortListOfConnections(std::list<Connection>& listOfConnections)
//{
//	std::sort(listOfConnections.begin(), listOfConnections.end(), compareOfNodeNumbers );
//}
