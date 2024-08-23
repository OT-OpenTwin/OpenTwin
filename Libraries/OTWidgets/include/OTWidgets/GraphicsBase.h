//! @file GraphicsBase.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qpoint.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsBase {
	public:
		GraphicsBase() {};
		virtual ~GraphicsBase() {};

		virtual qreal calculateClosestDistanceToPoint(const QPointF& _pt) const { return DBL_MAX; };
	};

}