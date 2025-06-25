//! @file CartesianPlotCurve.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

/*

The methods:
ot::CartesianPlotCurve::verifyRange
and
ot::CartesianPlotCurve::drawSeries
use the following license:

                             Qwt License
                           Version 1.0, January 1, 2003

The Qwt library and included programs are provided under the terms
of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL) with the following
exceptions:

    1. Widgets that are subclassed from Qwt widgets do not
       constitute a derivative work.

    2. Static linking of applications and widgets to the
       Qwt library does not constitute a derivative work
       and does not require the author to provide source
       code for the application or widget, use the shared
       Qwt libraries, or link their applications or
       widgets against a user-supplied version of Qwt.

       If you link the application or widget to a modified
       version of Qwt, then the changes to Qwt must be
       provided under the terms of the LGPL in sections
       1, 2, and 4.

    3. You do not have to provide a copy of the Qwt license
       with programs that are linked to the Qwt library, nor
       do you have to identify the Qwt license in your
       program or documentation as required by section 6
       of the LGPL.


       However, programs must still identify their use of Qwt.
       The following example statement can be included in user
       documentation to satisfy this requirement:

           [program/widget] is based in part on the work of
           the Qwt project (http://qwt.sf.net).


*/

#pragma once

// OpenTwin header
#include "OTWidgets/CartesianPlotCurve.h"

// Qwt header
#include <qwt_symbol.h>

// Qt header
#include <QtGui/qpainter.h>

int ot::CartesianPlotCurve::verifyRange(int _size, int& _i1, int& _i2) {
    if (_size < 1) {
        return 0;
    }

    _i1 = qBound(0, _i1, _size - 1);
    _i2 = qBound(0, _i2, _size - 1);

    if (_i1 > _i2) {
        qSwap(_i1, _i2);
    }

    return (_i2 - _i1 + 1);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor

ot::CartesianPlotCurve::CartesianPlotCurve(const QString& _title) :
	QwtPlotCurve(_title), m_outlinePen(Qt::NoPen), m_pointInterval(1)
{

}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::CartesianPlotCurve::drawSeries(QPainter* _painter, const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QRectF& _canvasRect, int _from, int _to) const {
    const size_t numSamples = this->dataSize();
    
	// Check if painter is valid and number of samples is positive
    if (!_painter || numSamples <= 0) {
        return;
    }

	// Ensure the range is valid
    if (_to < 0) {
        _to = numSamples - 1;
    }

    if (CartesianPlotCurve::verifyRange(numSamples, _from, _to) > 0) {
        _painter->save();

        // Draw outline if needed
		if (m_outlinePen.style() != Qt::NoPen) {
            _painter->setPen(m_outlinePen);
            this->drawCurve(_painter, this->style(), _xMap, _yMap, _canvasRect, _from, _to);
		}

		// Draw the curve
        _painter->setPen(this->pen());
        this->drawCurve(_painter, this->style(), _xMap, _yMap, _canvasRect, _from, _to);

        _painter->restore();

		// Draw symbols if they are set
        if (this->symbol() && (this->symbol()->style() != QwtSymbol::NoSymbol)) {
            _painter->save();
            this->drawSymbols(_painter, *this->symbol(), _xMap, _yMap, _canvasRect, _from, _to);
            _painter->restore();
        }
    }
}
