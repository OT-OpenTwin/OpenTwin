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
#include "OpenTwinFoundation/UiComponent.h"

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

 void Circuit::addConnection(const std::string& key,const Connection& obj)
{
	if (mapOfElements.find(key) != mapOfElements.end())
	{
		mapOfElements[key].addConnection(obj);
		 
	}
	else
	{
		Application::instance()->uiComponent()->displayMessage("Element not found"); // Auf OtLog umändern
		
	}

}

 //void Circuit::sort(const std::string& key)
 //{
	// if (mapOfElements.find(key) != mapOfElements.end())
	// {
	//	 mapOfElements[key].sortListOfConnections(mapOfElements[key].getList());
	// }
	// else
	// {
	//	 Application::instance()->uiComponent()->displayMessage("Element not found"); // Auf OtLog umändern
	// }
 //}

std::string Circuit::getId()
{
	return this->id;
}

std::map<std::string, CircuitElement> Circuit::getMapOfElements()
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

