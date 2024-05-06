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
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qpoint.h>
#include <QtCore/qsize.h>
#include <QtGui/qcolor.h>
#include <QtGui/qbrush.h>

namespace ot {

	class Painter2D;

	//! @class QtFactory
	//! @brief The QtFactory is used to convert OpenTwin configuration objects to Qt objects.
	class OT_WIDGETS_API_EXPORT QtFactory {
		OT_DECL_NOCOPY(QtFactory)
	public:
		//! @brief Convert OpenTwin Color to Qt Color
		static QColor toQt(const ot::Color& _color);
		static QColor toQt(const ot::ColorF& _color);

		static Qt::Alignment toQt(Alignment _alignment);

		static QPoint toQt(const Point2D& _pt);
		static QPointF toQt(const Point2DF& _pt);
		static QPointF toQt(const Point2DD& _pt);

		static QSize toQt(const Size2D& _s);
		static QSizeF toQt(const Size2DF& _s);
		static QSizeF toQt(const Size2DD& _s);

		static QGradient::Spread toQt(ot::GradientSpread _spread);

		//! @brief Creates a QBrush equivalent to the provided painter.
		//! Returns default if no painter provided.
		static QBrush toQt(const ot::Painter2D* _painter);

	private:
		QtFactory() {};
		~QtFactory() {};
	};
}