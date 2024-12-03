/*
 * Application.h
 *
 *  Created on: 03.12.2024
 *	Author: Sebastian Urmann
 *  Copyright (c)
 */


#pragma once

class Application
{
public:
	static Application* getInstance();

	static void deleteInstance(void);

	// Delete Copy & Move Constructors
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	//void initializeServiceLogNotifier();


private:
	Application();
	~Application();

	static Application* instance;
};
