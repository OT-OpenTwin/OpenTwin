//! @file DoubleSpinBox.h
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

	class OT_WIDGETS_API_EXPORT DoubleSpinBox : public QDoubleSpinBox, public QWidgetInterface {
		OT_DECL_NOCOPY(DoubleSpinBox)
	public:
		DoubleSpinBox(QWidget* _parentWidget = (QWidget*)nullptr);
		DoubleSpinBox(double _min, double _max, double _value, int _precision = 2, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~DoubleSpinBox();

		virtual QWidget* getQWidget(void) override { return this; };
	};

}