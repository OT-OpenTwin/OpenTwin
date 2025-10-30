// @otlicense
// File: Tool.cpp
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

// Toolkit API
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/BasicWidgetView.h"

otoolkit::Tool::Tool() 
	: m_isRunning(false), m_isExternal(false)
{

}

otoolkit::Tool::~Tool() {

}

ot::WidgetView* otoolkit::Tool::createCentralWidgetView(QWidget* _widget, const QString& _widgetName) const {
	ot::WidgetView* view = this->createWidgetView(_widget, _widgetName);
	if (!view) return nullptr;
	
	ot::WidgetViewBase vd = view->getViewData();
	vd.setViewFlag(ot::WidgetViewBase::ViewIsCentral);
	view->setViewData(vd);

	return view;
}

ot::WidgetView* otoolkit::Tool::createSideWidgetView(QWidget* _widget, const QString& _widgetName) const {
	ot::WidgetView* view = this->createWidgetView(_widget, _widgetName);
	if (!view) return nullptr;

	ot::WidgetViewBase vd = view->getViewData();
	vd.setViewFlag(ot::WidgetViewBase::ViewIsSide);
	view->setViewData(vd);
	
	return view;
}

ot::WidgetView* otoolkit::Tool::createToolWidgetView(QWidget* _widget, const QString& _widgetName) const {
	ot::WidgetView* view = this->createWidgetView(_widget, _widgetName);
	if (!view) return nullptr;

	ot::WidgetViewBase vd = view->getViewData();
	vd.setViewFlag(ot::WidgetViewBase::ViewIsTool);
	vd.setViewFlag(ot::WidgetViewBase::ViewIsPinnable);
	vd.setViewFlag(ot::WidgetViewBase::ViewIsCloseable);
	view->setViewData(vd);

	return view;
}

ot::WidgetView* otoolkit::Tool::createWidgetView(QWidget* _widget, const QString& _widgetName) const {
	if (!_widget) {
		OT_LOG_E("No widget provided");
		return nullptr;
	}
	ot::BasicWidgetView* ret = new ot::BasicWidgetView(_widget);

	ot::WidgetViewBase basicInformation(ot::WidgetViewBase::CustomView);
	basicInformation.setEntityName(this->getToolName().toStdString() + "_" + _widgetName.toStdString());
	basicInformation.setTitle(_widgetName.toStdString());
	ret->setViewData(basicInformation);

	return ret;
}