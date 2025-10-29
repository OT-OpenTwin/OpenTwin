// @otlicense

//! @file PlotPointMapper.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

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

// OpenTwin header
#include "OTWidgets/PlotPointMapper.h"

// Qwt header
#include <qwt_scale_map.h>
#include <qwt_series_data.h>
#include <qwt_pixel_matrix.h>

// Qt header
#include <QtCore/qpoint.h>
#include <QtCore/qthread.h>
#include <QtCore/qfuture.h>
#include <QtCore/qfuture.h>
#include <QtGui/qrgb.h>
#include <QtGui/qpen.h>
#include <QtGui/qimage.h>
#include <QtGui/qpainter.h>

namespace ot {
    namespace intern {
        static inline int roundValue(double _value) {
            return qRound(_value);
        }

        static inline double roundValueF(double _value) {
            return nearbyint(_value);
        }

        static Qt::Orientation probeOrientation(const QwtSeriesData<QPointF>* _series, int _from, int _to) {
            if (_to - _from < 20) {
                // not enough points to "have an orientation"
                return Qt::Horizontal;
            }

            const double x0 = _series->sample(_from).x();
            const double xn = _series->sample(_to).x();

            if (x0 == xn) {
                return Qt::Vertical;
            }

            const int step = (_to - _from) / 10;
            const bool isIncreasing = xn > x0;

            double x1 = x0;
            for (int i = _from + step; i < _to; i += step) {
                const double x2 = _series->sample(i).x();
                if (x2 != x1) {
                    if ((x2 > x1) != isIncreasing) {
                        return Qt::Vertical;
                    }
                }

                x1 = x2;
            }

            return Qt::Horizontal;
        }

        template<class Polygon, class Point>
        class PolygonQuadrupelX {
        public:
            inline void start(int _x, int _y) {
                m_x0 = _x;
                m_y1 = m_yMin = m_yMax = m_y2 = _y;
            }

            inline bool append(int _x, int _y) {
                if (m_x0 != _x) {
                    return false;
                }

                if (_y < m_yMin) {
                    m_yMin = _y;
                }
                else if (_y > m_yMax) {
                    m_yMax = _y;
                }

                m_y2 = _y;

                return true;
            }

            inline void flush(Polygon& _polyline) {
                appendTo(m_y1, _polyline);

                if (m_y2 > m_y1) {
                    qSwap(m_yMin, m_yMax);
                }

                if (m_yMax != m_y1) {
                    appendTo(m_yMax, _polyline);
                }

                if (m_yMin != m_yMax) {
                    appendTo(m_yMin, _polyline);
                }

                if (m_y2 != m_yMin) {
                    appendTo(m_y2, _polyline);
                }
            }

        private:
            inline void appendTo(int _y, Polygon& _polyline) {
                _polyline += Point(m_x0, _y);
            }

        private:
            int m_x0;
            int m_y1;
            int m_yMin;
            int m_yMax;
            int m_y2;
        };

        template<class Polygon, class Point>
        class PolygonQuadrupelY {
        public:
            inline void start(int _x, int _y) {
                m_y0 = _y;
                m_x1 = m_xMin = m_xMax = m_x2 = _x;
            }

            inline bool append(int _x, int _y) {
                if (m_y0 != _y) {
                    return false;
                }

                if (_x < m_xMin) {
                    m_xMin = _x;
                }
                else if (_x > m_xMax) {
                    m_xMax = _x;
                }

                m_x2 = _x;

                return true;
            }

            inline void flush(Polygon& _polyline) {
                appendTo(m_x1, _polyline);

                if (m_x2 > m_x1) {
                    qSwap(m_xMin, m_xMax);
                }

                if (m_xMax != m_x1) {
                    appendTo(m_xMax, _polyline);
                }

                if (m_xMin != m_xMax) {
                    appendTo(m_xMin, _polyline);
                }

                if (m_x2 != m_xMin) {
                    appendTo(m_x2, _polyline);
                }
            }

        private:
            inline void appendTo(int _x, Polygon& _polyline) {
                _polyline += Point(_x, m_y0);
            }

            int m_y0;
            int m_x1;
            int m_xMin;
            int m_xMax;
            int m_x2;
        };


        template<class Polygon, class Point, class PolygonQuadrupel>
        static Polygon mapPointsQuad(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to) {
            const QPointF sample0 = _series->sample(_from);

            PolygonQuadrupel q;
            q.start(roundValue(_xMap.transform(sample0.x())), roundValue(_yMap.transform(sample0.y())));

            Polygon polyline;
            for (int i = _from; i <= _to; i++) {
                const QPointF sample = _series->sample(i);

                const int x = roundValue(_xMap.transform(sample.x()));
                const int y = roundValue(_yMap.transform(sample.y()));

                if (!q.append(x, y)) {
                    q.flush(polyline);
                    q.start(x, y);
                }
            }
            q.flush(polyline);

            return polyline;
        }

        template<class Polygon, class Point, class PolygonQuadrupel>
        static Polygon mapPointsQuad(const Polygon& _polyline) {
            const int numPoints = _polyline.size();

            if (numPoints < 3) {
                return _polyline;
            }

            const Point* points = _polyline.constData();

            Polygon polylineXY;

            PolygonQuadrupel q;
            q.start(points[0].x(), points[0].y());

            for (int i = 0; i < numPoints; i++) {
                const int x = points[i].x();
                const int y = points[i].y();

                if (!q.append(x, y)) {
                    q.flush(polylineXY);
                    q.start(x, y);
                }
            }
            q.flush(polylineXY);

            return polylineXY;
        }


        template<class Polygon, class Point>
        static Polygon mapPointsQuad(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to) {
            Polygon polyline;
            if (_from > _to) {
                return polyline;
            }

            /*
                probing some values, to decide if it is better
                to start with x or y coordinates
             */
            const Qt::Orientation orientation = probeOrientation(_series, _from, _to);

            if (orientation == Qt::Horizontal) {
                polyline = mapPointsQuad<Polygon, Point, PolygonQuadrupelY<Polygon, Point>>(_xMap, _yMap, _series, _from, _to);

                polyline = mapPointsQuad<Polygon, Point, PolygonQuadrupelX<Polygon, Point>>(polyline);
            }
            else {
                polyline = mapPointsQuad<Polygon, Point, PolygonQuadrupelX<Polygon, Point>>(_xMap, _yMap, _series, _from, _to);

                polyline = mapPointsQuad<Polygon, Point, PolygonQuadrupelY<Polygon, Point>>(polyline);
            }

            return polyline;
        }

        class DotsCommand {
        public:
            const QwtSeriesData<QPointF>* series;
            int from;
            int to;
            QRgb rgb;
        };

        static void renderDots(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const DotsCommand& _command, const QPoint& _pos, QImage* _image) {
            const QRgb rgb = _command.rgb;
            QRgb* bits = reinterpret_cast<QRgb*>(_image->bits());

            const int w = _image->width();
            const int h = _image->height();

            const int x0 = _pos.x();
            const int y0 = _pos.y();

            for (int i = _command.from; i <= _command.to; i++) {
                const QPointF sample = _command.series->sample(i);

                const int x = static_cast<int>(_xMap.transform(sample.x()) + 0.5) - x0;
                const int y = static_cast<int>(_yMap.transform(sample.y()) + 0.5) - y0;

                if (x >= 0 && x < w && y >= 0 && y < h) {
                    bits[y * w + x] = rgb;
                }
            }
        }

        struct RoundI {
            inline int operator()(double _value) const {
                return roundValue(_value);
            }
        };

        struct RoundF {
            inline double operator()(double _value) const {
                return roundValueF(_value);
            }
        };

        struct NoRoundF {
            inline double operator()(double _value) const {
                return _value;
            }
        };

        template <class Polygon, class Point, class Round>
        static inline Polygon toPoints(const QRectF& _boundingRect, const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, PlotPointMapper::IntervalInfo& _interval, Round _round) {
            Polygon polyline(_to - _from + 1);
            Point* points = polyline.data();

            int numPoints = 0;

            if (_boundingRect.isValid()) {
                // iterating over all values
                // filtering out all points outside of
                // the bounding rectangle

                for (int i = _from; i <= _to; i++) {
                    if (_interval.count == 0) {
                        const QPointF sample = _series->sample(i);

                        const double x = _xMap.transform(sample.x());
                        const double y = _yMap.transform(sample.y());

                        if (_boundingRect.contains(x, y)) {
                            points[numPoints].rx() = round(x);
                            points[numPoints].ry() = round(y);

                            numPoints++;
                        }
                    }
                    if (++_interval.count >= _interval.limit) {
                        _interval.count = 0;
                    }
                }

                polyline.resize(numPoints);
            }
            else {
                // simply iterating over all values
                // without any filtering

                for (int i = _from; i <= _to; i++) {
                    if (_interval.count == 0) {
                        const QPointF sample = _series->sample(i);

                        const double x = _xMap.transform(sample.x());
                        const double y = _yMap.transform(sample.y());

                        points[numPoints].rx() = round(x);
                        points[numPoints].ry() = round(y);

                        numPoints++;
                    }
					if (++_interval.count >= _interval.limit) {
						_interval.count = 0;
					}
                }

                polyline.resize(numPoints);
            }

            return polyline;
        }

        static inline QPolygon toPointsI(const QRectF& _boundingRect, const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, PlotPointMapper::IntervalInfo& _interval) {
            return toPoints<QPolygon, QPoint>(_boundingRect, _xMap, _yMap, _series, _from, _to, _interval, RoundI());
        }

        template <class Round>
        static inline QPolygonF toPointsF(const QRectF& _boundingRect, const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, PlotPointMapper::IntervalInfo& _interval, Round _round) {
            return toPoints<QPolygonF, QPointF>(_boundingRect, _xMap, _yMap, _series, _from, _to, _interval, _round);
        }

        template <class Polygon, class Point, class Round>
        static inline Polygon toPolylineFiltered(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, PlotPointMapper::IntervalInfo& _interval, Round _round) {
            // in curves with many points consecutive points
            // are often mapped to the same position. As this might
            // result in empty lines ( or symbols hidden by others )
            // we try to filter them out

            Polygon polyline(_to - _from + 1);
            Point* points = polyline.data();

            const QPointF sample0 = _series->sample(_from);

            points[0].rx() = round(_xMap.transform(sample0.x()));
            points[0].ry() = round(_yMap.transform(sample0.y()));

            if (++_interval.count >= _interval.limit) {
                _interval.count = 0;
            }

            int pos = 0;
            for (int i = _from + 1; i <= _to; i++) {
                if (_interval.count == 0) {
                    const QPointF sample = _series->sample(i);

                    const Point p(round(_xMap.transform(sample.x())), round(_yMap.transform(sample.y())));

                    if (points[pos] != p) {
                        points[++pos] = p;
                    }
                }
                if (++_interval.count >= _interval.limit) {
                    _interval.count = 0;
                }
            }

            polyline.resize(pos + 1);
            return polyline;
        }

        static inline QPolygon toPolylineFilteredI(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, PlotPointMapper::IntervalInfo& _interval) {
            return toPolylineFiltered<QPolygon, QPoint>(_xMap, _yMap, _series, _from, _to, _interval, RoundI());
        }

        template <class Round>
        static inline QPolygonF toPolylineFilteredF(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, PlotPointMapper::IntervalInfo& _interval, Round _round) {
            return toPolylineFiltered<QPolygonF, QPointF>(_xMap, _yMap, _series, _from, _to, _interval, _round);
        }

        template <class Polygon, class Point>
        static inline Polygon toPointsFiltered(const QRectF& _boundingRect, const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, PlotPointMapper::IntervalInfo& _interval) {
            // F.e. in scatter plots ( no connecting lines ) we
            // can sort out all duplicates ( not only consecutive points )

            Polygon polygon(_to - _from + 1);
            Point* points = polygon.data();

            QwtPixelMatrix pixelMatrix(_boundingRect.toAlignedRect());

            int numPoints = 0;
            for (int i = _from; i <= _to; i++) {
                if (_interval.count == 0) {
                    const QPointF sample = _series->sample(i);

                    const int x = roundValue(_xMap.transform(sample.x()));
                    const int y = roundValue(_yMap.transform(sample.y()));

                    if (pixelMatrix.testAndSetPixel(x, y, true) == false) {
                        points[numPoints].rx() = x;
                        points[numPoints].ry() = y;

                        numPoints++;
                    }
                }
                if (++_interval.count >= _interval.limit) {
                    _interval.count = 0;
                }
            }

            polygon.resize(numPoints);
            return polygon;
        }

        static inline QPolygon toPointsFilteredI(const QRectF& _boundingRect, const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, PlotPointMapper::IntervalInfo& _interval) {
            return toPointsFiltered<QPolygon, QPoint>(_boundingRect, _xMap, _yMap, _series, _from, _to, _interval);
        }

        static inline QPolygonF toPointsFilteredF(const QRectF& _boundingRect, const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, PlotPointMapper::IntervalInfo& _interval) {
            return toPointsFiltered<QPolygonF, QPointF>(_boundingRect, _xMap, _yMap, _series, _from, _to, _interval);
        }

    }
}

// Static helper

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::PlotPointMapper::PlotPointMapper() :
	m_boundingRect(0.0, 0.0, -1.0, -1.0)
{}

ot::PlotPointMapper::~PlotPointMapper() {}

void ot::PlotPointMapper::setFlag(QwtPointMapper::TransformationFlag _flag, bool _on) {
    if (_on) {
        m_transformationFlags |= _flag;
    }
    else {
        m_transformationFlags &= ~_flag;
    }
}

bool ot::PlotPointMapper::testFlag(QwtPointMapper::TransformationFlag _flag) const {
    return m_transformationFlags & _flag;
}

QPolygonF ot::PlotPointMapper::toPolygonF(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, IntervalInfo& _interval) const {
    QPolygonF polyline;

    if (m_transformationFlags & QwtPointMapper::RoundPoints) {
        if (m_transformationFlags & QwtPointMapper::WeedOutIntermediatePoints) {
            polyline = intern::mapPointsQuad<QPolygonF, QPointF>(_xMap, _yMap, _series, _from, _to);
        }
        else if (m_transformationFlags & QwtPointMapper::WeedOutPoints) {
            polyline = intern::toPolylineFilteredF(_xMap, _yMap, _series, _from, _to, _interval, intern::RoundF());
        }
        else {
            polyline = intern::toPointsF(QRectF(0., 0., -1., -1), _xMap, _yMap, _series, _from, _to, _interval, intern::RoundF());
        }
    }
    else {
        if (m_transformationFlags & QwtPointMapper::WeedOutPoints) {
            polyline = intern::toPolylineFilteredF(_xMap, _yMap, _series, _from, _to, _interval, intern::NoRoundF());
        }
        else {
            polyline = intern::toPointsF(QRectF(0., 0., -1., -1.), _xMap, _yMap, _series, _from, _to, _interval, intern::NoRoundF());
        }
    }

    return polyline;
}

QPolygon ot::PlotPointMapper::toPolygon(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, IntervalInfo& _interval) const {
    QPolygon polyline;

    if (m_transformationFlags & QwtPointMapper::WeedOutIntermediatePoints) {
        // TODO WeedOutIntermediatePointsY ...
        polyline = intern::mapPointsQuad<QPolygon, QPoint>(_xMap, _yMap, _series, _from, _to);
    }
    else if (m_transformationFlags & QwtPointMapper::WeedOutPoints) {
        polyline = intern::toPolylineFilteredI(_xMap, _yMap, _series, _from, _to, _interval);
    }
    else {
        polyline = intern::toPointsI(QRectF(0., 0., -1., -1.), _xMap, _yMap, _series, _from, _to, _interval);
    }

    return polyline;
}

QPolygonF ot::PlotPointMapper::toPointsF(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, IntervalInfo& _interval) const {
    QPolygonF points;

    if (m_transformationFlags & QwtPointMapper::WeedOutPoints) {
        if (m_transformationFlags & QwtPointMapper::RoundPoints) {
            if (m_boundingRect.isValid()) {
                points = intern::toPointsFilteredF(m_boundingRect, _xMap, _yMap, _series, _from, _to, _interval);
            }
            else {
                // without a bounding rectangle all we can
                // do is to filter out duplicates of
                // consecutive points

                points = intern::toPolylineFilteredF(_xMap, _yMap, _series, _from, _to, _interval, intern::RoundF());
            }
        }
        else {
            // when rounding is not allowed we can't use
            // qwtToPointsFilteredF

            points = intern::toPolylineFilteredF(_xMap, _yMap, _series, _from, _to, _interval, intern::NoRoundF());
        }
    }
    else {
        if (m_transformationFlags & QwtPointMapper::RoundPoints) {
            points = intern::toPointsF(m_boundingRect, _xMap, _yMap, _series, _from, _to, _interval, intern::RoundF());
        }
        else {
            points = intern::toPointsF(m_boundingRect, _xMap, _yMap, _series, _from, _to, _interval, intern::NoRoundF());
        }
    }

    return points;
}

QPolygon ot::PlotPointMapper::toPoints(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, IntervalInfo& _interval) const {
    QPolygon points;

    if (m_transformationFlags & QwtPointMapper::WeedOutPoints) {
        if (m_boundingRect.isValid()) {
            points = intern::toPointsFilteredI(m_boundingRect, _xMap, _yMap, _series, _from, _to, _interval);
        }
        else {
            // when we don't have the bounding rectangle all
            // we can do is to filter out consecutive duplicates

            points = intern::toPolylineFilteredI(_xMap, _yMap, _series, _from, _to, _interval);
        }
    }
    else {
        points = intern::toPointsI(m_boundingRect, _xMap, _yMap, _series, _from, _to, _interval);
    }

    return points;
}

QImage ot::PlotPointMapper::toImage(const QwtScaleMap& _xMap, const QwtScaleMap& _yMap, const QwtSeriesData<QPointF>* _series, int _from, int _to, const QPen& _pen, bool _antialiased, uint _numThreads) const {
    Q_UNUSED(_antialiased)

#if QWT_USE_THREADS
    if (_numThreads == 0) {
        _numThreads = QThread::idealThreadCount();
    }

    if (_numThreads <= 0) {
        _numThreads = 1;
    }
#else
    Q_UNUSED(_numThreads)
#endif

    // a very special optimization for scatter plots
    // where every sample is mapped to one pixel only.

    const QRect rect = m_boundingRect.toAlignedRect();

    QImage image(rect.size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    if (_pen.width() <= 1 && _pen.color().alpha() == 255) {
        intern::DotsCommand command;
        command.series = _series;
        command.rgb = _pen.color().rgba();

#if QWT_USE_THREADS
        const int numPoints = (_to - _from + 1) / _numThreads;

        QList<QFuture<void>> futures;
        for (uint i = 0; i < _numThreads; i++) {
            const QPoint pos = rect.topLeft();

            const int index0 = _from + i * numPoints;
            if (i == _numThreads - 1) {
                command.from = index0;
                command.to = _to;

                intern::renderDots(_xMap, _yMap, command, pos, &image);
            }
            else {
                command.from = index0;
                command.to = index0 + numPoints - 1;

                futures += QtConcurrent::run(&intern::renderDots,
                    _xMap, _yMap, command, pos, &image);
            }
        }
        for (int i = 0; i < futures.size(); i++)
            futures[i].waitForFinished();
#else
        command.from = _from;
        command.to = _to;

        intern::renderDots(_xMap, _yMap, command, rect.topLeft(), &image);
#endif
    }
    else {
        // fallback implementation: to be replaced later by
        // setting the pixels of the image like above, TODO ...

        QPainter painter(&image);
        painter.setPen(_pen);
        painter.setRenderHint(QPainter::Antialiasing, _antialiased);

        const int chunkSize = 1000;
        PlotPointMapper::IntervalInfo interval;
        for (int i = _from; i <= _to; i += chunkSize) {
            const int indexTo = qMin(i + chunkSize - 1, _to);
            const QPolygon points = toPoints(_xMap, _yMap, _series, i, indexTo, interval);

            painter.drawPoints(points);
        }
    }

    return image;
}
