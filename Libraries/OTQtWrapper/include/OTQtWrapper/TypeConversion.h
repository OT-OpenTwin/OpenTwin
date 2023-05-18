//! @file TypeConversion.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/Color.h"

// Qt header
#include <QtGui/qcolor.h>

namespace ot {

	inline QColor& operator << (QColor& _lhv, const ot::Color& _rhv) { _lhv.setRgb(_rhv.rInt(), _rhv.gInt(), _rhv.bInt(), _rhv.aInt()); return _lhv; };
	inline QColor& operator >> (const ot::Color& _lhv, QColor& _rhv) { _rhv.setRgb(_lhv.rInt(), _lhv.gInt(), _lhv.bInt(), _lhv.aInt()); return _rhv; };

	inline ot::Color& operator << (ot::Color& _lhv, const QColor& _rhv) { _lhv.set(_rhv.red(), _rhv.green(), _rhv.blue(), _rhv.alpha()); return _lhv; };
	inline ot::Color& operator >> (const QColor& _lhv, ot::Color& _rhv) { _rhv.set(_lhv.red(), _lhv.green(), _lhv.blue(), _lhv.alpha()); return _rhv; };

	inline QColor colorToQColor(const ot::Color& _color) { QColor r; r << _color; return r; };
	inline ot::Color colorFromQColor(const QColor& _color) { ot::Color r; r << _color; return r; };

}