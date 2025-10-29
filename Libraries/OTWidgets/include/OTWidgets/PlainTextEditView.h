// @otlicense

//! @file PlainTextEditView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"

namespace ot {

	class PlainTextEdit;

	class OT_WIDGETS_API_EXPORT PlainTextEditView : public WidgetView {
		OT_DECL_NOCOPY(PlainTextEditView)
	public:
		PlainTextEditView(PlainTextEdit* _textEdit = (PlainTextEdit*)nullptr);
		virtual ~PlainTextEditView();

		// ###########################################################################################################################################################################################################################################################################################################################

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		PlainTextEdit* getPlainTextEdit(void) const { return m_textEdit; };

	private:
		PlainTextEdit* m_textEdit;
	};

}