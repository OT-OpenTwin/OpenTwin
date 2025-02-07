//! @file SVGWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/SVGWidget.h"

ot::SVGWidget::SVGWidget(const QString& _imagePath, QWidget* parent)
	: QSvgWidget(_imagePath, parent)
{

}
