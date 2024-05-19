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
		static QColor toColor(const ot::Color& _color);
		static QColor toColor(const ot::ColorF& _color);

		static Qt::Alignment toAlignment(Alignment _alignment);

		static QPoint toPoint(const Point2D& _pt);
		static QPointF toPoint(const Point2DF& _pt);
		static QPointF toPoint(const Point2DD& _pt);

		static QSize toSize(const Size2D& _s);
		static QSizeF toSize(const Size2DF& _s);
		static QSizeF toSize(const Size2DD& _s);

		static QGradient::Spread toGradientSpread(ot::GradientSpread _spread);

		//! @brief Creates a QBrush equivalent to the provided painter.
		//! Returns default if no painter provided.
		static QBrush toBrush(const ot::Painter2D* _painter);

		static Qt::PenStyle toPenStyle(LineStyle _style);
		static Qt::PenCapStyle toPenCapStyle(LineCapStyle _style);
		static Qt::PenJoinStyle toPenJoinStyle(LineJoinStyle _style);

		static QPen toPen(const Outline& _outline);
		static QPen toPen(const OutlineF& _outline);

		static QPainterPath toPainterPath(const Path2DF& _path);

		static QString toString(QEvent::Type _type);

	private:
		QtFactory() {};
		~QtFactory() {};
	};
}