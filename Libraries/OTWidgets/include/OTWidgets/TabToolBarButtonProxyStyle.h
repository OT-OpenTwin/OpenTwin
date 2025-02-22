//! @file TabToolBarButtonProxyStyle.h
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qproxystyle.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT TabToolBarButtonProxyStyle : public QProxyStyle {
	public:
		void drawControl(ControlElement _element, const QStyleOption* _opt, QPainter* _painter, const QWidget* _widget) const override;
		void drawComplexControl(ComplexControl _control, const QStyleOptionComplex* _opt, QPainter* _painter, const QWidget* _widget) const override;
	};

}