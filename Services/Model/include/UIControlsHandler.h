#pragma once
#include "Application.h"

#include "OTServiceFoundation/MenuButtonDescription.h"
#include <atomic>

class UIControlsHandler
{
public:
	UIControlsHandler();
	
	const std::atomic_bool& getUICreated() { return m_uiCreated; }

private:
	std::atomic_bool m_uiCreated = false;

	const std::string m_pageModel = "Model";

	const std::string m_groupGeometry = "Geometry";
	
	const std::string m_groupParameters = "Parameters";
	const std::string m_groupEdit = "Edit";
	const std::string m_groupImport = "File Imports";
	
	ot::MenuButtonDescription m_buttonGeometryInfo;

	ot::MenuButtonDescription m_buttonCreateParameter;
	
	ot::MenuButtonDescription m_buttonRedo;
	ot::MenuButtonDescription m_buttonUndo;
	ot::MenuButtonDescription m_buttonDelete;
	
	void createPages();
	void createGroups();
	void setupButtons();
	void createButtons();
	
};
