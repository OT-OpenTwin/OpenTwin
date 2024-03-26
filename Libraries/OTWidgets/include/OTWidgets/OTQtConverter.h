//! @file OTQtConverter.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#ifndef OT_OTQTCONVERTER_H
#define OT_OTQTCONVERTER_H

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/Point2D.h"
#include "OTCore/Size2D.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qpoint.h>
#include <QtCore/qsize.h>
#include <QtGui/qcolor.h>
#include <QtGui/qbrush.h>

namespace ot {
	class OT_WIDGETS_API_EXPORT OTQtConverter {
		OT_DECL_NOCOPY(OTQtConverter)
	public:
		//! @brief Convert OpenTwin Color to Qt Color
		static QColor toQt(const ot::Color& _color);

		static Qt::Alignment toQt(Alignment _alignment);

		static QPoint toQt(const Point2D& _pt);
		static QPointF toQt(const Point2DF& _pt);
		static QPointF toQt(const Point2DD& _pt);

		static QSize toQt(const Size2D& _s);
		static QSizeF toQt(const Size2DF& _s);
		static QSizeF toQt(const Size2DD& _s);

		static QGradient::Spread toQt(ot::GradientSpread _spread);

	private:
		OTQtConverter() {};
		~OTQtConverter() {};
	};
}

#endif