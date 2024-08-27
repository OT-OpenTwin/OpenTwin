//! @file ImagePainter.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qrect.h>
#include <QtCore/qsize.h>

class QPainter;

namespace ot {

	class ImagePainter {
		OT_DECL_NOCOPY(ImagePainter)
	public:
		ImagePainter() {};
		virtual ~ImagePainter() {};

		virtual void paintImage(QPainter* _painter, const QRectF& _bounds) const = 0;

		virtual QSizeF getDefaultImageSize(void) const = 0;

	};

}