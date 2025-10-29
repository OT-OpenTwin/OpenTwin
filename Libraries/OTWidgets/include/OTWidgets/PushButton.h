// @otlicense

//! @file PushButton.h
//! @author Alexander Kuester (alexk95)
//! @date January 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qpushbutton.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PushButton : public QPushButton, public WidgetBase {
		Q_OBJECT
	public:
		PushButton(QWidget* _parent = (QWidget*)nullptr);
		PushButton(const QString& _text, QWidget* _parent = (QWidget*)nullptr);
		PushButton(const QIcon& _icon, const QString& _text, QWidget* _parent = (QWidget*)nullptr);
		virtual ~PushButton() {};

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

	public Q_SLOTS:
		void setSelectedProperty(void);
		void unsetSelectedProperty(void);
	};

}