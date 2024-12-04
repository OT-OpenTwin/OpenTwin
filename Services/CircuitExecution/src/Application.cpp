/*
 * Application.cpp
 *
 *  Created on: 03.12.2024
 *	Author: Sebastian Urmann
 *  Copyright (c)
 */

#include "Application.h"


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


void Application::start(const std::string& serverName)
{
	QString qStr = QString::fromStdString(serverName);
	ConnectionManager m_connectionManager;
	m_connectionManager.connectToCircuitSimulatorService(qStr);
	
}

Application::Application() {

}

Application::~Application()
{

}

