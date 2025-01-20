//! @file PlainTextEdit.h
//! @author Alexander Kuester (alexk95)
//! @date December 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtWidgets/qplaintextedit.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PlainTextEdit : public QPlainTextEdit, public ot::QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(PlainTextEdit)
	public:
		PlainTextEdit(QWidget* _parent = (QWidget*)nullptr);
		virtual ~PlainTextEdit();

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		void setAutoScrollToBottomEnabled(bool _enabled);
		bool isAutoScrollToBottomEnabled(void) const { return m_autoScrollToBottom; }

		void scrollToBottom(void);

	public Q_SLOTS:
		void slotTextChanged(void);

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void keyReleaseEvent(QKeyEvent* _event) override;

	private:
		bool m_autoScrollToBottom;

	};

}
