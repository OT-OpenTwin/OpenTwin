// @otlicense
// File: TestToolBar.h
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

// Qt header
#include <QtWidgets/qtoolbar.h>

class WidgetTest;

namespace ot {
	class MainWindow;
	class CentralWidgetManager;
	class TabToolBar;
}

class TestToolBar : public QToolBar
{
public:
	TestToolBar(WidgetTest* _owner);

public Q_SLOTS:
	void slotTest();

private:
	WidgetTest* m_owner;

	ot::MainWindow* m_window;
	ot::CentralWidgetManager* m_centralWidgetManager;
	ot::TabToolBar* m_toolBarManager;
};
