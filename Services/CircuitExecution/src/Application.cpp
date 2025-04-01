/*
 * Application.cpp
 *
 *  Created on: 03.12.2024
 *	Author: Sebastian Urmann
 *  Copyright (c)
 */

#include "Application.h"
#include "ConnectionManager.h"

Application* Application::instance = nullptr;

Application* Application::getInstance()
{
	if (instance == nullptr) {
		instance = new Application();
	}
	
	return instance;
}

void Application::deleteInstance(void)
{
	if (instance) {
		delete instance;
	}
	instance = nullptr;
}


void Application::start(const std::string& _serverName)
{	
	
	QString qStr = QString::fromStdString(_serverName);
	m_connectionManager = new ConnectionManager();
	m_connectionManager->connectToCircuitSimulatorService(qStr);

	
	
}

ConnectionManager* Application::getConnectionManager() {
	return m_connectionManager;
}

Application::Application() {
	m_connectionManager = nullptr;

}

Application::~Application()
{

}

