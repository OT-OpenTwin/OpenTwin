/*

This class is a modified version of the QwtPointMapper and therefore uses the following license:

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
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_point_mapper.h>

// Qt header
#include <QtCore/qrect.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PlotPointMapper {
		OT_DECL_NOCOPY(PlotPointMapper)
        OT_DECL_DEFMOVE(PlotPointMapper)
    public:
        class IntervalInfo {
        public:
            IntervalInfo() : limit(1), count(0) {};
			IntervalInfo(int _limit) : limit(_limit), count(0) {};

            const int limit;
            int count;
        };

        PlotPointMapper();
        ~PlotPointMapper();

        void setFlags(QwtPointMapper::TransformationFlags _flags) { m_transformationFlags = _flags; };
        QwtPointMapper::TransformationFlags flags() const { return m_transformationFlags; };

        void setFlag(QwtPointMapper::TransformationFlag _flag, bool on = true);
        bool testFlag(QwtPointMapper::TransformationFlag _flag) const;

		void setBoundingRect(const QRectF& _rect) { m_boundingRect = _rect; };
        QRectF boundingRect() const { return m_boundingRect; };

        QPolygonF toPolygonF(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, IntervalInfo& _interval) const;

        QPolygon toPolygon(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, IntervalInfo& _interval) const;

        QPolygon toPoints(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, IntervalInfo& _interval) const;

        QPolygonF toPointsF(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, IntervalInfo& _interval) const;

        QImage toImage(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, const QPen&, bool _antialiased, uint _numThreads) const;

    private:
		QRectF m_boundingRect;
        QwtPointMapper::TransformationFlags m_transformationFlags;
	};

}