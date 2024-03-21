//! @file PlainTextEditView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/PlainTextEditView.h"
#include "OTWidgets/WidgetViewRegistrar.h"

static ot::WidgetViewRegistrar<ot::PlainTextEditView> PlainTextEditViewRegistrar(OT_WIDGETTYPE_PlainTextEditView);

ot::PlainTextEditView::PlainTextEditView() {
	this->addWidgetToDock(this);
}

ot::PlainTextEditView::~PlainTextEditView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

bool ot::PlainTextEditView::setupFromConfig(WidgetViewCfg* _config) {
	if (!WidgetView::setupFromConfig(_config)) return false;
	PlainTextEditViewCfg* config = dynamic_cast<PlainTextEditViewCfg*>(_config);
	if (!config) {
		OT_LOG_E("Configuration cast failed");
		return false;
	}

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################
