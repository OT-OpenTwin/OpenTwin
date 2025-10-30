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

// Open twin header
#include "OTGuiAPI/ButtonHandler.h"
#include "OTServiceFoundation/ApplicationBase.h"		// Base class
#include "EntityInformation.h"
#include "EntityGeometry.h"

// C++ header
#include <string>
#include <list>
#include <map>
#include <sstream>
#include <complex>

// Forward declaration
class EntityBase;
class EntityUnits;
class ParametricCombination;

class Application : public ot::ApplicationBase, public ot::ButtonHandler {
public:
	Application();
	virtual ~Application();

	// ##################################################################################################################################

	// Required functions

	//! @brief Will be called whenever a action should be processed. Core actions will be processed in the base and will not be forwarded to this function (see documentation)
	//! @param _action The action that should be processed
	//! @param _doc The document containing all the information
	virtual std::string processAction(const std::string & _action, ot::JsonDocument& _doc) override;

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	// ##################################################################################################################################

	virtual void modelSelectionChanged() override;

	void importProject(void);
	void setLTSpiceFile(void);
	void showInformation(void);
	void commitChanges(void);
	void getChanges(void);
	void setLocalFileName(const std::string& hostName, const std::string& fileName);

	void writeProjectInformation(const std::string &simpleFileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames);
	bool readProjectInformation(std::string &simpleFileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames);
	bool isProjectInitialized();
	std::string getLocalFileName(const std::string& hostName);
	void addHostNameAndFileName(const std::string& hostName, const std::string& fileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames);
	std::string getSimpleFileName();
	long long getCurrentModelEntityVersion(void);
	void extractResults();

private:
	void uploadNeeded(ot::JsonDocument& _doc);
	void downloadNeeded(ot::JsonDocument& _doc);
	void filesUploaded(ot::JsonDocument& _doc);
};
