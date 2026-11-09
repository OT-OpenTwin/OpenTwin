// @otlicense
// File: UIContextMenuManager.h
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

// OpenTwin header
#include "OTCore/BasicEntityInformation.h"
#include "OTGui/Menu/MenuCfg.h"
#include "OTGui/Event/MenuRequestData.h"
#include "OTWidgets/Menu/MenuManagerHandler.h"
#include "OTWidgets/Menu/MenuManager.h"

// Qt header
#include <QtCore/qobject.h>

class AppBase;

namespace ak {
	class aTreeWidget;
}
namespace ot {
	class MenuRequestEvent;
	class MenuCallbackBase;
	class RequestWidgetEvent;
}

class UIContextMenuManager : public ot::MenuManager, public ot::MenuManagerHandler
{
	Q_OBJECT
private:
	AppBase* m_app;

public:
	UIContextMenuManager();
	virtual ~UIContextMenuManager();

	void initialize(AppBase* _app);

	virtual ot::BasicEntityInformation getEntityInformationFromName(const std::string& _entityName, bool* _failFlag = (bool*)nullptr) const override;

protected:
	virtual ot::MenuCfg getMenuConfiguration(const ot::MenuCallbackBase* _callbackObject, const ot::MenuRequestEvent& _requestEvent) const override;
	virtual bool notifyMenuOwner(const ot::MenuAction* _action) const override;
	virtual bool triggerToolbarButton(const std::string& _buttonName) const override;
	virtual bool checkToolBarButtonExists(const std::string& _buttonName) const override;
	virtual bool isToolBarButtonEnabled(const std::string& _buttonName) const override;
};