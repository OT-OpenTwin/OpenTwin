/*
 * Application.h
 *
 *  Created on: 03.12.2024
 *	Author: Sebastian Urmann
 *  Copyright (c)
 */

//std Header
#include <string>


#pragma once

class ConnectionManager;

class Application
{
public:
	static Application* getInstance();

	static void deleteInstance(void);

	// Delete Copy & Move Constructors
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void start(const std::string& _serverName);


private:
	Application();
	~Application();

	static Application* instance;
	ConnectionManager* m_connectionManager;
};
