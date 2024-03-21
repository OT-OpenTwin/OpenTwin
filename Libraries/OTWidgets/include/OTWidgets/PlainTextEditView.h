//! @file PlainTextEditView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PlainTextEditViewCfg.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/PlainTextEdit.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT PlainTextEditView : public PlainTextEdit, public WidgetView {
		OT_DECL_NOCOPY(PlainTextEditView)
	public:
		PlainTextEditView();
		virtual ~PlainTextEditView();

		// ###########################################################################################################################################################################################################################################################################################################################

		virtual QWidget* getViewWidget(void) override { return this; };

		virtual bool setupFromConfig(WidgetViewCfg* _config) override;

		// ###########################################################################################################################################################################################################################################################################################################################

	private:

	};

}