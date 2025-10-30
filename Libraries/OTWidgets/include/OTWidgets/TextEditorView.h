// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"

namespace ot {

	class TextEditor;

	class OT_WIDGETS_API_EXPORT TextEditorView : public WidgetView {
		Q_OBJECT
	public:
		TextEditorView(TextEditor* _textEditor = (TextEditor*)nullptr);
		virtual ~TextEditorView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		TextEditor* getTextEditor(void) const { return m_textEditor; };

	Q_SIGNALS:
		void saveRequested(void);

	private Q_SLOTS:
		void slotSaveRequested(void);
		void slotModifiedChanged(bool _isModified);
		
	private:
		TextEditor* m_textEditor;

	};

}
