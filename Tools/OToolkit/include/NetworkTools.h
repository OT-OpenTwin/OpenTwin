// @otlicense
// File: NetworkTools.h
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
#include "OTCore/OTClassHelper.h"
#include "OToolkitAPI/Tool.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <list>

class PortBlockerServer;
namespace ot { class ComboBox; }
namespace ot { class PushButton; }
namespace ot { class PlainTextEdit; }

class NetworkTools : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	enum NetworkToolStatus {
		Idle               = 0x00,
		PortBlockerRunning = 0x01
	};
	typedef ot::Flags<NetworkToolStatus> NetworkToolStatusFlags;

	NetworkTools();
	virtual ~NetworkTools();

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString getToolName(void) const override { return QString("Network Tools"); };

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	virtual void restoreToolSettings(QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

	// ###########################################################################################################################################################################################################################################################################################################################

private Q_SLOTS:
	void slotRunPortBlocker(void);
	
private:
	NetworkToolStatusFlags m_status;

	std::list<PortBlockerServer*> m_servers;
	ot::ComboBox* m_portBlockerType;
	ot::PushButton* m_runPortBlockerButton;
	ot::PlainTextEdit* m_portBlockerPorts;
	
};

OT_ADD_FLAG_FUNCTIONS(NetworkTools::NetworkToolStatus)