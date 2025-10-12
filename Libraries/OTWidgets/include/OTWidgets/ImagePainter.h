//! @file ImagePainter.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qrect.h>
#include <QtCore/qsize.h>

class QPainter;

namespace ot {

	class OT_WIDGETS_API_EXPORT ImagePainter {
		OT_DECL_NOCOPY(ImagePainter)
	public:
		ImagePainter() {};
		virtual ~ImagePainter() {};

		virtual void paintImage(QPainter* _painter, const QRectF& _bounds) const = 0;

		virtual QSizeF getDefaultImageSize(void) const = 0;

	};

}