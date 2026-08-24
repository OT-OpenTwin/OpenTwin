// @otlicense
// File: ToolViewManager.h
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
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

// std header
#include <map>
#include <list>

class ToolRuntimeHandler;
namespace ot { class WidgetView; };

class ToolViewManager : public QObject{
	Q_OBJECT
	OT_DECL_NOCOPY(ToolViewManager)
public:
	ToolViewManager();
	virtual ~ToolViewManager();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Tool management

	void addTool(ToolRuntimeHandler* _handler);

	void removeTool(const QString& _toolName);

	void updateViews(const QString& _toolName);

	void addIgnoredView(ot::WidgetView* _view) { m_ignoredViews.push_back(_view); };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter / Finder

	ToolRuntimeHandler* findTool(const QString& _toolName) const;
	ToolRuntimeHandler* findTool(ot::WidgetView* _view) const;

Q_SIGNALS:
	void viewFocused(const QString& _viewName, const QString& _toolName);
	void viewFocusLost(const QString& _viewName, const QString& _toolName);
	void viewCloseRequested(const QString& _viewName, const QString& _toolName);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Slots

public Q_SLOTS:
	void slotViewFocusChanged(ot::WidgetView* _focused, ot::WidgetView* _previousView);
	void slotViewCloseRequested(ot::WidgetView* _view);

private:
	bool isViewIgnored(ot::WidgetView* _view) const;

	std::map<QString, ToolRuntimeHandler*> m_nameMap;
	std::map<ot::WidgetView*, ToolRuntimeHandler*> m_viewMap;
	std::list<ot::WidgetView*> m_ignoredViews;
	
};