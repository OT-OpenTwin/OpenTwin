// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qplaintextedit.h>

namespace ot {

	class CustomValidator;

	class OT_WIDGETS_API_EXPORT PlainTextEdit : public QPlainTextEdit, public ot::WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PlainTextEdit)
	public:
		PlainTextEdit(QWidget* _parent = (QWidget*)nullptr);
		virtual ~PlainTextEdit();

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		void setAutoScrollToBottomEnabled(bool _enabled);
		bool isAutoScrollToBottomEnabled() const { return m_autoScrollToBottom; }

		void setValidator(CustomValidator* _validator);
		CustomValidator* getValidator() const { return m_validator; };

		void scrollToBottom();

	public Q_SLOTS:
		void slotTextChanged();

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void keyReleaseEvent(QKeyEvent* _event) override;

	private:
		CustomValidator* m_validator;
		bool m_autoScrollToBottom;

	};

}
