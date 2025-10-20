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

	class OT_WIDGETS_API_EXPORTONLY ImagePainter {
		OT_DECL_NOMOVE(ImagePainter)
	public:
		ImagePainter() = default;
		ImagePainter(const ImagePainter&) = default;
		virtual ~ImagePainter() = default;

		ImagePainter& operator=(const ImagePainter&) = default;

		virtual void paintImage(QPainter* _painter, const QRect& _bounds, bool _maintainAspectRatio) const = 0;
		virtual void paintImage(QPainter* _painter, const QRectF& _bounds, bool _maintainAspectRatio) const = 0;

		virtual ImagePainter* createCopy() const = 0;

		virtual QSize getDefaultImageSize() const = 0;
		virtual QSizeF getDefaultImageSizeF() const = 0;

	};

}