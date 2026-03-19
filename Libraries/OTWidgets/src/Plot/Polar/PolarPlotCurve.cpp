// @otlicense
// File: PolarPlotCurve.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end


/*

The methods:
    ot::intern::qwtInsidePole,
	ot::intern::qwtVerifyRange,
    ot::PolarPlotCurve::drawCurve,
    ot::PolarPlotCurve::drawSymbols and
    ot::PolarPlotCurve::drawLines
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

// OpenTwin header
#include "OTWidgets/Plot/Polar/PolarPlotCurve.h"

// Qwt header
#include <qwt_symbol.h>
#include <qwt_scale_map.h>
#include <qwt_curve_fitter.h>
#include <qwt_clipper.h>
#include <qwt_painter.h>

namespace ot {
    namespace intern {
        static inline bool qwtInsidePole(const QwtScaleMap& _map, double _radius)
        {
            return _map.isInverting() ? (_radius > _map.s1()) : (_radius < _map.s1());
        }

        static int qwtVerifyRange(int _size, int& _i1, int& _i2)
        {
            if (_size < 1)
            {
                return 0;
            }

            _i1 = qBound(0, _i1, _size - 1);
            _i2 = qBound(0, _i2, _size - 1);

            if (_i1 > _i2)
            {
                qSwap(_i1, _i2);
            }

            return (_i2 - _i1 + 1);
        }
    }
}

ot::PolarPlotCurve::PolarPlotCurve(const QString& _title) :
	QwtPolarCurve(_title), m_highlightPen(Qt::NoPen), m_pointInterval(1), m_hasHighlight(false)
{

}

void ot::PolarPlotCurve::setHighlight(bool _highlight) {
	if (_highlight == m_hasHighlight) {
		return;
	}

	m_hasHighlight = _highlight;
}

void ot::PolarPlotCurve::drawCurve(QPainter* _painter, int _style, const QwtScaleMap& _azimuthMap, const QwtScaleMap& _radialMap, const QPointF& _pole, int _from, int _to) const
{
    switch (_style)
    {
    case Lines:
        drawLines(_painter, _azimuthMap, _radialMap, _pole, _from, _to);
        break;
    case NoCurve:
    default:
        break;
    }
}

void ot::PolarPlotCurve::drawSymbols(QPainter* _painter, const QwtSymbol& _symbol, const QwtScaleMap& _azimuthMap, const QwtScaleMap& _radialMap, const QPointF& _pole, int _from, int _to) const
{
    _painter->setBrush(_symbol.brush());
    _painter->setPen(_symbol.pen());

    const int chunkSize = 500;

    for (int i = _from; i <= _to; i += chunkSize)
    {
        const int n = std::min(chunkSize, _to - i + 1);

        QPolygonF points;
        for (int j = 0; j < n; j++)
        {
            const QwtPointPolar point = sample(i + j);

            if (!intern::qwtInsidePole(_radialMap, point.radius()))
            {
                const double r = _radialMap.transform(point.radius());
                const double a = _azimuthMap.transform(point.azimuth());

                points += qwtPolar2Pos(_pole, r, a);
            }
            else
            {
                points += _pole;
            }
        }

        if (points.size() > 0)
        {
            _symbol.drawSymbols(_painter, points);
        }
    }
}

void ot::PolarPlotCurve::drawLines(QPainter* _painter, const QwtScaleMap& _azimuthMap, const QwtScaleMap& _radialMap, const QPointF& _pole, int _from, int _to) const
{
    int size = _to - _from + 1;
    if (size <= 0)
    {
        return;
    }

    QPolygonF polyline;
    
    auto fitter = curveFitter();

    if (fitter)
    {
        QPolygonF points(size);
        for (int j = _from; j <= _to; j++)
        {
            const QwtPointPolar point = sample(j);
            points[j - _from] = QPointF(point.azimuth(), point.radius());
        }

        points = fitter->fitCurve(points);

        polyline.resize(points.size());

        QPointF* polylineData = polyline.data();
        QPointF* pointsData = points.data();

        for (int i = 0; i < points.size(); i++)
        {
            const QwtPointPolar point(pointsData[i].x(), pointsData[i].y());

            double r = _radialMap.transform(point.radius());
            const double a = _azimuthMap.transform(point.azimuth());

            polylineData[i] = qwtPolar2Pos(_pole, r, a);
        }
    }
    else
    {
        polyline.resize(size);
        QPointF* polylineData = polyline.data();

        for (int i = _from; i <= _to; i++)
        {
            QwtPointPolar point = sample(i);
            if (!intern::qwtInsidePole(_radialMap, point.radius()))
            {
                double r = _radialMap.transform(point.radius());
                const double a = _azimuthMap.transform(point.azimuth());
                polylineData[i - _from] = qwtPolar2Pos(_pole, r, a);
            }
            else
            {
                polylineData[i - _from] = _pole;
            }
        }
    }

    QRectF clipRect;
    if (_painter->hasClipping())
    {
        clipRect = _painter->clipRegion().boundingRect();
    }
    else
    {
        clipRect = _painter->window();
        if (!clipRect.isEmpty())
        {
            clipRect = _painter->transform().inverted().mapRect(clipRect);
        }
    }

    if (!clipRect.isEmpty())
    {
        double off = qCeil(std::max(qreal(1.0), _painter->pen().widthF()));
        clipRect = clipRect.toRect().adjusted(-off, -off, off, off);
        QwtClipper::clipPolygonF(clipRect, polyline);
    }

    if (m_hasHighlight) {
        _painter->save();
		_painter->setPen(m_highlightPen);
        QwtPainter::drawPolyline(_painter, polyline);
		_painter->restore();
	}

    QwtPainter::drawPolyline(_painter, polyline);
}
