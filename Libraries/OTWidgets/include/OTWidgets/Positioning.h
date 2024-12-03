//! \file Positioning.h
//! \author Alexander Kuester (alexk95)
//! \date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qrect.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT Positioning {
		OT_DECL_NOCOPY(Positioning)
		OT_DECL_NODEFAULT(Positioning)
	public:
		//! \brief Calculates the child rect.
		//! \param _parentRect Parent rectangle.
		//! \param _childSize Child size.
		//! \param _childAlignment Child alignment.
		static QRect calculateChildRect(const QRect& _parentRect, const QSize& _childSize, ot::Alignment _childAlignment);

		//! \brief Calculates the child rect.
		//! \param _parentRect Parent rectangle.
		//! \param _childSize Child size.
		//! \param _childAlignment Child alignment.
		static QRectF calculateChildRect(const QRectF& _parentRect, const QSizeF& _childSize, ot::Alignment _childAlignment);

		//! \brief Fits the provided rect on the screen.
		//! If the source rect is bigger than the screen size the source rect will be returned.
		//! \param _sourceRect Initial rect.
		//! \param _primaryScreenOnly If true the source rect will be fitted onto the primary screen only, otherwise on any screen.
		static QRect fitOnScreen(const QRect& _sourceRect, bool _primaryScreenOnly = false);

	};
}