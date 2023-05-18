//! @file TypeConversion.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTQtWrapper/OTQtWrapperAPIExport.h"
#include "OpenTwinCore/Color.h"
#include "OTGui/Font.h"

// Qt header
#include <QtGui/qfont.h>
#include <QtGui/qcolor.h>

namespace ot {

	// Color

	inline QColor& operator << (QColor& _lhv, const ot::Color& _rhv) { _lhv.setRgb(_rhv.rInt(), _rhv.gInt(), _rhv.bInt(), _rhv.aInt()); return _lhv; };
	inline QColor& operator >> (const ot::Color& _lhv, QColor& _rhv) { return _rhv << _lhv; };

	inline ot::Color& operator << (ot::Color& _lhv, const QColor& _rhv) { _lhv.set(_rhv.red(), _rhv.green(), _rhv.blue(), _rhv.alpha()); return _lhv; };
	inline ot::Color& operator >> (const QColor& _lhv, ot::Color& _rhv) { return _rhv << _lhv; };

	inline QColor colorToQColor(const ot::Color& _color) { QColor r; r << _color; return r; };
	inline ot::Color colorFromQColor(const QColor& _color) { ot::Color r; r << _color; return r; };

	// Font

	OT_QTWRAPPER_API_EXPORT QFont& operator << (QFont& _lhv, const ot::Font& _rhv);
	inline QFont& operator >> (const ot::Font& _lhv, QFont& _rhv) { return _rhv << _lhv; };

	OT_QTWRAPPER_API_EXPORT ot::Font& operator << (ot::Font& _lhv, const QFont& _rhv);
	inline ot::Font& operator >> (const QFont& _lhv, ot::Font& _rhv) { return _rhv << _lhv; };

	inline QFont fontToQFont(const ot::Font& _font) { QFont f; f << _font; return f; };
	inline ot::Font fontFromQFont(const QFont& _font) { ot::Font f; f << _font; return f; };
}
