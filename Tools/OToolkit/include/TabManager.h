//! @file TabManager.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/OTClassHelper.h"
#include "OTWidgets/TabWidget.h"

// std header
#include <map>

class TabManager : public ot::TabWidget {
	OT_DECL_NOCOPY(TabManager)
public:
	TabManager();
	virtual ~TabManager();

	void addTool(const QString& _toolName, QWidget* _toolWidget);
	void removeTool(const QString& _toolName);

private:
	std::map<QString, QWidget*> m_data;
};
