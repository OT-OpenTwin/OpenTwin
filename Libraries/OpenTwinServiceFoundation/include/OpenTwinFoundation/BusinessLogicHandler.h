/*****************************************************************//**
 * \file   BusinessLogicHandler.h
 * \brief  Common methods for all central service handler classes.
 * 
 * \author Wagner
 * \date   April 2023
 *********************************************************************/
#pragma once
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include "OpenTwinFoundation/FoundationAPIExport.h"

#include <string>
#include <vector>
#include <list>

class __declspec(dllexport) BusinessLogicHandler
{
	friend class FixtureBusinessLogicHandler;
public:
	void setUIComponent(ot::components::UiComponent * ui) 
	{ 
		_uiComponent = ui; 
		UIComponentWasSet();
	};
	void setModelComponent(ot::components::ModelComponent * model) 
	{
		_modelComponent = model;
		ModelComponentWasSet();
	};
	virtual ~BusinessLogicHandler() {};

protected:
	ot::components::UiComponent * _uiComponent = nullptr;
	ot::components::ModelComponent * _modelComponent = nullptr;

	inline void CheckEssentials();

	std::string CreateNewUniqueTopologyName(std::list<std::string>& folderContent, const std::string& folderName, const std::string& fileName, int startNumber = 1, bool alwaysNumbered = false);
	std::string CreateNewUniqueTopologyName(const std::string& folderName, const std::string& fileName, int startNumber = 1, bool alwaysNumbered = false);
	std::vector<std::string> CreateNewUniqueTopologyNames(std::list<std::string>& folderContent, const std::string& folderName, const std::string& fileName, uint64_t numberOfFiles, int startNumber = 1, bool alwaysNumbered = false);
	std::vector<std::string> CreateNewUniqueTopologyNames(const std::string& folderName, const std::string& fileName, uint64_t numberOfFiles, int startNumber = 1, bool alwaysNumbered = false);
		
	virtual void UIComponentWasSet() {};
	virtual void ModelComponentWasSet() {};
};

