//! @file OTQtConverter.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#ifndef OT_OTQTCONVERTER_H
#define OT_OTQTCONVERTER_H

// OpenTwin header
#include "OpenTwinCore/Color.h"
#include "OpenTwinCore/Point2D.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qpoint.h>
#include <QtGui/qcolor.h>
#include <QtWidgets/qsizepolicy.h>

namespace ot {
	namespace OTQtConverter {

		//! @brief Convert OpenTwin Color to Qt Color
		OT_WIDGETS_API_EXPORTONLY QColor toQt(const ot::Color& _color);

		OT_WIDGETS_API_EXPORTONLY Qt::Alignment toQt(ot::Alignment _alignment);

		OT_WIDGETS_API_EXPORTONLY QPoint toQt(const ot::Point2D& _pt);
		OT_WIDGETS_API_EXPORTONLY QPointF toQt(const ot::Point2DF& _pt);
		OT_WIDGETS_API_EXPORTONLY QPointF toQt(const ot::Point2DD& _pt);
		
		OT_WIDGETS_API_EXPORTONLY QSizePolicy toQt(ot::SizePolicy _policy);
	}
}

#endif