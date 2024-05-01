//! @file Tool.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

// Toolkit API
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/BasicWidgetView.h"

otoolkit::Tool::Tool() {

}

otoolkit::Tool::~Tool() {

}

ot::WidgetView* otoolkit::Tool::createCentralWidgetView(QWidget* _widget, const QString& _widgetName) const {
	ot::WidgetView* view = this->createWidgetView(_widget, _widgetName);
	if (!view) return nullptr;
	
	view->viewData().setFlag(ot::WidgetViewBase::ViewIsCentral);
	
	return view;
}

ot::WidgetView* otoolkit::Tool::createSideWidgetView(QWidget* _widget, const QString& _widgetName) const {
	ot::WidgetView* view = this->createWidgetView(_widget, _widgetName);
	if (!view) return nullptr;

	view->viewData().setFlag(ot::WidgetViewBase::ViewIsSide);
	
	return view;
}

ot::WidgetView* otoolkit::Tool::createWidgetView(QWidget* _widget, const QString& _widgetName) const {
	if (!_widget) {
		OT_LOG_E("No widget provided");
		return nullptr;
	}
	ot::BasicWidgetView* ret = new ot::BasicWidgetView(_widget);

	ot::WidgetViewBase basicInformation;
	basicInformation.setName(this->toolName().toStdString() + "_" + _widgetName.toStdString());
	basicInformation.setTitle(_widgetName.toStdString());
	ret->setViewData(basicInformation);

	return ret;
}