// @otlicense
// File: Application.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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

