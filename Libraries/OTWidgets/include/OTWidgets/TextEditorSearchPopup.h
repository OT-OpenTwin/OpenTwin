//! @file TextEditorSearchPopup.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OverlayWidgetBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	class LineEdit;
	class TextEditor;

	class OT_WIDGETS_API_EXPORT TextEditorSearchPopup : public OverlayWidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(TextEditorSearchPopup)
		OT_DECL_NODEFAULT(TextEditorSearchPopup)
	public:
		TextEditorSearchPopup(TextEditor* _editor);

		virtual ~TextEditorSearchPopup() {};

		void focusInput(void);

		virtual bool eventFilter(QObject* _watched, QEvent* _event) override;

		virtual void closeEvent(QCloseEvent* _event) override;

		void setIndex(int _ix) { m_ix = _ix; };

		void updatePosition(bool _forceUpdate = false);

	Q_SIGNALS:
		void popupClosing(void);

	private Q_SLOTS:
		void slotTextChanged(void);

		void slotFindNext(void);

	private:
		int m_ix;
		QPoint m_lastTR;
		TextEditor* m_editor;
		LineEdit* m_search;
	};
}