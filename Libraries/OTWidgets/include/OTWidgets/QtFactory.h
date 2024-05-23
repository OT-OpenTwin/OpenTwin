//! @file QtFactory.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/Size2D.h"
#include "OTCore/Color.h"
#include "OTCore/Point2D.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/Outline.h"
#include "OTGui/Path2D.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qpoint.h>
#include <QtCore/qsize.h>
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
		static constexpr inline QColor toColor(const ot::Color& _color) noexcept { return std::move(QColor(_color.r(), _color.g(), _color.b(), _color.a())); };
		static constexpr inline QColor toColor(const ot::ColorF& _color) noexcept { return std::move(QColor((int)(_color.r() * 255.f), (int)(_color.g() * 255.f), (int)(_color.b() * 255.f), (int)(_color.a() * 255.f))); };

		static constexpr inline QPoint toPoint(const Point2D& _pt) noexcept { return std::move(QPoint(_pt.x(), _pt.y())); };
		static constexpr inline QPointF toPoint(const Point2DF& _pt) noexcept { return std::move(QPointF((qreal)_pt.x(), (qreal)_pt.y())); };
		static constexpr inline QPointF toPoint(const Point2DD& _pt) noexcept { return std::move(QPointF((qreal)_pt.x(), (qreal)_pt.y())); };

		static constexpr inline QSize toSize(const Size2D& _s) noexcept { return std::move(QSize(_s.width(), _s.height())); };
		static constexpr inline QSizeF toSize(const Size2DF& _s) noexcept { return std::move(QSizeF((qreal)_s.width(), (qreal)_s.height())); };
		static constexpr inline QSizeF toSize(const Size2DD& _s) noexcept { return std::move(QSizeF((qreal)_s.width(), (qreal)_s.height())); };

		static inline QPen toPen(const Outline& _outline) { return std::move(QPen(toBrush(_outline.painter()), (qreal)_outline.width(), toPenStyle(_outline.style()), toPenCapStyle(_outline.cap()), toPenJoinStyle(_outline.joinStyle()))); };
		static inline QPen toPen(const OutlineF& _outline) { return std::move(QPen(toBrush(_outline.painter()), _outline.width(), toPenStyle(_outline.style()), toPenCapStyle(_outline.cap()), toPenJoinStyle(_outline.joinStyle()))); };

		static Qt::Alignment toAlignment(Alignment _alignment);

		static QGradient::Spread toGradientSpread(ot::GradientSpread _spread);

		//! @brief Creates a QBrush equivalent to the provided painter.
		//! Returns default if no painter provided.
		static QBrush toBrush(const ot::Painter2D* _painter);

		static Qt::PenStyle toPenStyle(LineStyle _style);
		static Qt::PenCapStyle toPenCapStyle(LineCapStyle _style);
		static Qt::PenJoinStyle toPenJoinStyle(LineJoinStyle _style);

		static QPainterPath toPainterPath(const Path2DF& _path);

		static QString toString(QEvent::Type _type);

	private:
		QtFactory() {};
		~QtFactory() {};
	};
}