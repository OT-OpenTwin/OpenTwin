// @otlicense
// File: Application.h
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

#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/exception.hpp>

//std Header
#include <string>
#include <thread>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

// Service header
#include "StartArgumentParser.h"


// OpenTwin header
#include "OTModelEntities/Lms/LibraryElement.h"

class Application {
public:
	static Application* getInstance();

	static void deleteInstance(void);

	// Delete Copy & Move Constructors
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void start(ot::StartArgumentParser _argumentParser);


private:
	Application();
	~Application();

	std::list<ot::LibraryElement> getLocalModels(const std::string& _modelFolderPath, const std::string& _collectionName);
	void fillLibraryElementWithHash(ot::LibraryElement& _element, const std::string& _modelFolderPath);
	std::list<ot::LibraryElement> addDataToLibraryElements(const std::list<ot::LibraryElement>& _elements, const std::string& _modelFolderPath);

    void createJsonDocumentFromLibraryElement(std::list<ot::LibraryElement> _element, ot::JsonDocument& _doc);
	std::list<ot::LibraryElement> createLibraryElementsFromJsonDocument(const std::string& _lmsResponse);

	std::string sendToLms(const ot::JsonDocument& _doc, std::string _lmsUrl);
	std::string sendAsyncToLms(const ot::JsonDocument& _doc, std::string _lmsUrl);

	const std::string m_collectionName = "";
	std::string m_databasePWD;
	std::string m_folderPath;


	static Application* instance;
};