//! @file Painter2DFactory.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtGui/qbrush.h>

namespace ot {

	class Painter2D;

	namespace Painter2DFactory {
		QBrush brushFromPainter2D(ot::Painter2D* _painter);
	}
}