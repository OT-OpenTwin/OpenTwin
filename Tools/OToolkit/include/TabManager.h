//! @file TabManager.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/TabWidget.h"

// std header
#include <map>

class TabManager : public ot::TabWidget {
	Q_OBJECT
	OT_DECL_NOCOPY(TabManager)
public:
	TabManager();
	virtual ~TabManager();

	void addTool(const QString& _toolName, QWidget* _toolWidget);
	void removeTool(const QString& _toolName);

signals:
	void currentToolChanged(const QString& _toolName);

private slots:
	void slotTabChanged(int _ix);

private:
	int m_currentIx;
	std::map<QString, QWidget*> m_data;
};
