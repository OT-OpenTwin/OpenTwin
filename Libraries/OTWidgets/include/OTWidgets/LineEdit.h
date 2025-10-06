//! @file LineEdit.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qlineedit.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT LineEdit : public QLineEdit, public ot::WidgetBase {
		Q_OBJECT
	public:
		LineEdit(QWidget* _parentWidget = (QWidget*)nullptr);
		LineEdit(const QString& _initialText, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~LineEdit() {};

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

	public Q_SLOTS:
		void setInputErrorStateProperty(void);
		void unsetInputErrorStateProperty(void);
		void setInputFocus(void);

	Q_SIGNALS:
		void leftMouseButtonPressed(void);

	protected:
		virtual void mousePressEvent(QMouseEvent* _event) override;
	};
}