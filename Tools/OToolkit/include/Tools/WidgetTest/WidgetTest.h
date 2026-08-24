// @otlicense
// File: WidgetTest.h
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
