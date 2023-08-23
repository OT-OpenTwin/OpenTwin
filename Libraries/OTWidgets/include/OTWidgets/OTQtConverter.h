//! @file OTQtConverter.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#ifndef OT_OTQTCONVERTER_H
#define OT_OTQTCONVERTER_H

// OpenTwin header
#include "OpenTwinCore/Color.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtGui/qcolor.h>

namespace ot {
	namespace OTQtConverter {

		//! @brief Convert OpenTwin Color to Qt Color
		OT_WIDGETS_API_EXPORTONLY QColor toQt(const ot::Color& _color);

	}
}

#endif