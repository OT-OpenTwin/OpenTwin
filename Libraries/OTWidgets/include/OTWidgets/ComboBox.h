//! @file ComboBox.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qcombobox.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT ComboBox : public QComboBox, public QWidgetInterface {
	public:
		ComboBox(QWidget* _parent = (QWidget*)nullptr);
		virtual ~ComboBox() {};

		virtual QWidget* getQWidget(void) override { return this; };
	};

}
