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

		//! \brief Calculates and returns the shortest distance to the given point.
		//! Returns -1 if the distance is invalid (e.g. maximum distance exceeded).
		//! \param _pt Point in scene coordinates.
		virtual qreal calculateShortestDistanceToPoint(const QPointF& _pt) const { return -1.; };

	};

}