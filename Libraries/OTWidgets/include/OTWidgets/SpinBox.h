//! @file SpinBox.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qspinbox.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT SpinBox : public QSpinBox, public QWidgetInterface {
		OT_DECL_NOCOPY(SpinBox)
	public:
		SpinBox(QWidget* _parentWidget = (QWidget*)nullptr);
		SpinBox(int _min, int _max, int _value, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~SpinBox();

		virtual QWidget* getQWidget(void) override { return this; };
	};

}