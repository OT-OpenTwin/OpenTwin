// @otlicense

//! @file CheckBox.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qcheckbox.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT CheckBox : public QCheckBox, public WidgetBase {
		Q_OBJECT
	public:
		CheckBox(QWidget* _parent = (QWidget*)nullptr);
		CheckBox(const QString& _text, QWidget* _parent = (QWidget*)nullptr);
		CheckBox(Qt::CheckState _state, const QString& _text, QWidget* _parent = (QWidget*)nullptr);
		virtual ~CheckBox() {};

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };
	};

}
