//! @file LineEdit.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtWidgets/qlineedit.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT LineEdit : public QLineEdit, public ot::QWidgetInterface {
	public:
		LineEdit(QWidget* _parentWidget = (QWidget*)nullptr);
		LineEdit(const QString& _initialText, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~LineEdit() {};

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getWidget(void) override { return this; };

	private:

	};
}