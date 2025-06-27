//! @file CartesianPlotCurve.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

/*

The methods:
    ot::CartesianPlotCurve::verifyRange,
    ot::CartesianPlotCurve::intersectedClipRect,
    ot::CartesianPlotCurve::drawSeries and
    ot::CartesianPlotCurve::drawSymbols
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
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_plot_curve.h>

// Qt header
#include <QtGui/qpen.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT CartesianPlotCurve : public QwtPlotCurve {
	public:
		static int verifyRange(int _size, int& _i1, int& _i2);

		static QRectF intersectedClipRect(const QRectF& _rect, QPainter* _painter);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor

		CartesianPlotCurve(const QString& _title = QString());

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setOutlinePen(const QPen& _pen) { m_outlinePen = _pen; };
		const QPen& getOutlinePen() const { return m_outlinePen; };

		void setPointInterval(int _interval) { m_pointInterval = _interval; };
		int getPointInterval() const { return m_pointInterval; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

	protected:

		virtual void drawSeries(QPainter* _painter, const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QRectF& _canvasRect, int _from, int _to) const override;

		virtual void drawSymbols(QPainter* _painter, const QwtSymbol& _symbol, const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QRectF& _canvasRect, int _from, int _to) const;

	private:
		QPen m_outlinePen;
		int m_pointInterval;
	};

}