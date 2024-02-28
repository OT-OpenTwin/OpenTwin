/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */




//Service Header
#include "Circuit.h"
#include "Application.h" 
#include "OTServiceFoundation/UiComponent.h"

//C++ Header
#include <string>


//Constructor
Circuit::Circuit()
{
	
}

Circuit::~Circuit() {}



std::string Circuit::getEditorName()
{
	return this->editorName;
}



std::string Circuit::getId()
{
	return this->id;
}

std::map<std::string, CircuitElement>& Circuit::getMapOfElements()
{
	return this->mapOfElements;
}

void Circuit::addElement(std::string key, const CircuitElement& obj)
{
	mapOfElements[key] = obj;
}

void Circuit::setEditorName(const std::string name)
{
	this->editorName = name;
}

void Circuit::setId(std::string id)
{
	this->id = id;
}

bool Circuit::addConnection(const std::string& key, const Connection& obj)
{
	if (mapOfElements.find(key) != mapOfElements.end())
	{
		bool result = mapOfElements[key].addConnection(obj);
		return result;

	}
	else
	{
		Application::instance()->uiComponent()->displayMessage("Element not found"); // Auf OtLog umändern

	}

}

std::string Circuit::findElement(const std::string& key)
{
	if (mapOfElements.find(key) != mapOfElements.end())
	{
		return mapOfElements[key].getItemName();
	}
	else
	{
		return "Not Found!";
	}
}
