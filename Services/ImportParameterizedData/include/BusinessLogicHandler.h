/*****************************************************************//**
 * \file   BusinessLogicHandler.h
 * \brief  Common methods for all central handler classes.
 * 
 * \author Wagner
 * \date   April 2023
 *********************************************************************/
#pragma once
#include <OpenTwinFoundation/UiComponent.h>
#include <OpenTwinFoundation/ModelComponent.h>

#include <string>
#include <vector>

class BusinessLogicHandler
{
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

	virtual void UIComponentWasSet() {};
	virtual void ModelComponentWasSet() {};
	void CheckEssentials();
	std::string CreateNewUniqueTopologyName(std::string folderName, std::string fileName);
	std::string CreateNewUniqueTopologyNamePlainPossible(std::string folderName, std::string fileName);
	std::vector<std::string> CreateNewUniqueTopologyName(std::string FolderName, std::string fileName, int numberOfFiles);
};

