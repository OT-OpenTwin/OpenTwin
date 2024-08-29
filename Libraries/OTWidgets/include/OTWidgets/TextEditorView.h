//! @file TextEditorView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/TextEditor.h"

#define OT_WIDGETTYPE_TextEditor "TextEditor"

namespace ot {

	class OT_WIDGETS_API_EXPORT TextEditorView : public TextEditor, public WidgetView {
	public:
		TextEditorView();
		virtual ~TextEditorView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		virtual bool setupViewFromConfig(WidgetViewCfg* _config) override;

		// ###########################################################################################################################################################################################################################################################################################################################

	protected:
		virtual void contentSaved(void) override;
		virtual void contentChanged(void) override;

	private:

	};

}
