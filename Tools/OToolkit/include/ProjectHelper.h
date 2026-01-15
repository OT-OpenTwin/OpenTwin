// @otlicense
// File: ProjectHelper.h
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

// Toolkit header
#include "ProjectHelperBatchHelper.h"

// OpenTwin header
#include "OTCore/Rect.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/Font.h"
#include "OTGui/PenCfg.h"
#include "OTGui/Margins.h"
#include "OTGui/VersionGraphVersionCfg.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qregularexpression.h>

// std header
#include <list>

class ProjectHelper : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	ProjectHelper() = default;
	virtual ~ProjectHelper() = default;

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	virtual QString getToolName(void) const override { return QString("Project Helper"); };
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	virtual void restoreToolSettings(QSettings& _settings) override;
	virtual bool prepareToolShutdown(QSettings& _settings) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void slotRefillBatches();

private:
	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

	ProjectHelperBatchHelper* m_batchHelper = nullptr;
};
