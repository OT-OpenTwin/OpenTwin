//! @file QtFactory.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Rect.h"
#include "OTCore/Color.h"
#include "OTCore/Size2D.h"
#include "OTCore/Point2D.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/Path2D.h"
#include "OTGui/Outline.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qrect.h>
#include <QtCore/qsize.h>
#include <QtCore/qpoint.h>
#include <QtCore/qstringlist.h>
#include <QtGui/qpen.h>
#include <QtGui/qcolor.h>
#include <QtGui/qbrush.h>
#include <QtGui/qevent.h>
#include <QtGui/qpainterpath.h>

namespace ot {

	class Painter2D;

	//! @class QtFactory
	//! @brief The QtFactory is used to convert OpenTwin configuration objects to Qt objects.
	class OT_WIDGETS_API_EXPORT QtFactory {
		OT_DECL_NOCOPY(QtFactory)
	public:
		//! @brief Convert OpenTwin Color to Qt Color
		static inline QColor toQColor(const ot::Color& _color) noexcept { return QColor(_color.r(), _color.g(), _color.b(), _color.a()); };
		static inline QColor toQColor(const ot::ColorF& _color) noexcept { return QColor((int)(_color.r() * 255.f), (int)(_color.g() * 255.f), (int)(_color.b() * 255.f), (int)(_color.a() * 255.f)); };

		static inline ot::Color toColor(const QColor& _color) { return ot::Color(_color.red(), _color.green(), _color.blue(), _color.alpha()); };
		static inline ot::ColorF toColorF(const QColor& _color) { return ot::Color(_color.red(), _color.green(), _color.blue(), _color.alpha()).toColorF(); };

		static inline QPoint toQPoint(const Point2D& _pt) noexcept { return QPoint(_pt.x(), _pt.y()); };
		static inline QPointF toQPoint(const Point2DF& _pt) noexcept { return QPointF((qreal)_pt.x(), (qreal)_pt.y()); };
		static inline QPointF toQPoint(const Point2DD& _pt) noexcept { return QPointF((qreal)_pt.x(), (qreal)_pt.y()); };

		static inline ot::Point2D toPoint2D(const QPoint& _pt) { return ot::Point2D(_pt.x(), _pt.y()); };
		static inline ot::Point2DD toPoint2D(const QPointF& _pt) { return ot::Point2DD(_pt.x(), _pt.y()); };

		static inline QSize toQSize(const Size2D& _s) noexcept { return QSize(_s.width(), _s.height()); };
		static inline QSizeF toQSize(const Size2DF& _s) noexcept { return QSizeF((qreal)_s.width(), (qreal)_s.height()); };
		static inline QSizeF toQSize(const Size2DD& _s) noexcept { return QSizeF((qreal)_s.width(), (qreal)_s.height()); };

		static inline Size2D toSize2D(const QSize& _s) { return Size2D(_s.width(), _s.height()); };
		static inline Size2DD toSize2D(const QSizeF& _s) { return Size2DD(_s.width(), _s.height()); };

		static inline QRect toQRect(const Rect& _r) noexcept { return QRect(toQPoint(_r.getTopLeft()), toQPoint(_r.getBottomRight())); };
		static inline QRectF toQRect(const RectF& _r) noexcept { return QRectF(toQPoint(_r.getTopLeft()), toQPoint(_r.getBottomRight())); };
		static inline QRectF toQRect(const RectD& _r) noexcept { return QRectF(toQPoint(_r.getTopLeft()), toQPoint(_r.getBottomRight())); };

		static inline Rect toRect(const QRect& _r) { return Rect(toPoint2D(_r.topLeft()), toPoint2D(_r.bottomRight())); };
		static inline RectD toRect(const QRectF& _r) { return RectD(toPoint2D(_r.topLeft()), toPoint2D(_r.bottomRight())); };

		static QPen toQPen(const Outline& _outline);
		static QPen toQPen(const OutlineF& _outline);

		static Qt::Alignment toQAlignment(Alignment _alignment);

		static QGradient::Spread toQGradientSpread(ot::GradientSpread _spread);

		//! @brief Creates a QBrush equivalent to the provided painter.
		//! Returns default if no painter provided.
		static QBrush toQBrush(const ot::Painter2D* _painter);

		static Qt::PenStyle toQPenStyle(LineStyle _style);
		static Qt::PenCapStyle toQPenCapStyle(LineCapStyle _style);
		static Qt::PenJoinStyle toQPenJoinStyle(LineJoinStyle _style);

		static QPainterPath toQPainterPath(const Path2DF& _path);

		static QString toQString(QEvent::Type _type);

	private:
		QtFactory() {};
		~QtFactory() {};
	};
}