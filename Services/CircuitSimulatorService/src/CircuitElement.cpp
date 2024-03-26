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

ot::UID CircuitElement::getUID() { return this->Uid; }

std::string CircuitElement::getValue() { return this->value; }

std::set<Connection>& CircuitElement::getList() { return listOfConnections; }

std::string CircuitElement::getType()
{
	return this->type;
}

std::string CircuitElement::getFunction()
{
	return this->function;
}

//Setter
void CircuitElement::setItemName(std::string name)	{this->m_itemName = name;}

void CircuitElement::setEditorName(std::string name)	{this->m_editorName = name;}

void CircuitElement::setUID(ot::UID id) { this->Uid = id; }

void CircuitElement::setValue(std::string name) { this->value = name; }

void CircuitElement::setType(std::string type)
{
	this->type = type;
}

void CircuitElement::setFunction(std::string function)
{
	this->function = function;
}

//Additional Functions

bool CircuitElement::addConnection(const Connection& obj) 
{
	auto result = listOfConnections.insert(obj);
	return result.second;
}


