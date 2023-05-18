//! @file PainterFactory.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"

// Qt header
#include <QtGui/qbrush.h>

namespace ot {

	class Painter2D;
	class FillPainter2D;
	class LinearGradientPainter2D;

	namespace PainterFactory {

		BLOCK_EDITOR_API_EXPORT QBrush brushFromPainter(ot::Painter2D * _painter);
		BLOCK_EDITOR_API_EXPORT QBrush brushFromPainter(ot::FillPainter2D* _painter);
		BLOCK_EDITOR_API_EXPORT QBrush brushFromPainter(ot::LinearGradientPainter2D* _painter);

	}

}