//! @file Math.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreAPIExport.h"

namespace ot {

	class OT_CORE_API_EXPORT Math {
	public:
		//! \brief Calculates the shortest distance from the given point to the given line.
		//! \param _px Point X.
		//! \param _py Point Y.
		//! \param _x1 Line Start X.
		//! \param _y1 Line Start Y.
		//! \param _x2 Line End X.
		//! \param _y2 Line End Y.
		static double calculateShortestDistanceFromPointToLine(double _px, double _py, double _x1, double _y1, double _x2, double _y2);
	};

}