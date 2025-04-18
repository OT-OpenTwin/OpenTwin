//! @file WidgetTest.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API header
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/VersionGraphVersionCfg.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot { class VersionGraphManager; }

class WidgetTest : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	WidgetTest() : m_versionGraph(nullptr) {};
	virtual ~WidgetTest() {};

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString getToolName(void) const override { return QString("Widget Test"); };

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	ot::VersionGraphManager* getVersionGraph(void) const { return m_versionGraph; };

private Q_SLOTS:
	void slotVersionDeselected(void);
	void slotVersionSelected(const std::string& _versionName);
	void slotVersionActivatRequest(const std::string& _versionName);

private:
	void updateVersionConfig(const ot::VersionGraphVersionCfg& _version);

	ot::VersionGraphManager* m_versionGraph;

};
