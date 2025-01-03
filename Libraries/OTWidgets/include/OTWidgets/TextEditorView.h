//! @file TextEditorView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/TextEditor.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT TextEditorView : public TextEditor, public WidgetView {
	public:
		TextEditorView();
		virtual ~TextEditorView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		virtual void setupFromConfig(const TextEditorCfg& _config, bool _isUpdate) override;

		// ###########################################################################################################################################################################################################################################################################################################################

	protected:
		virtual void contentSaved(void) override;
		virtual void contentChanged(void) override;

	private:

	};

}
