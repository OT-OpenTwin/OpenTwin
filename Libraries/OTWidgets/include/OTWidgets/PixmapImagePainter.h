//! @file PixmapImagePainter.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/ImagePainter.h"

// Qt header
#include <QtGui/qpixmap.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PixmapImagePainter : public ImagePainter {
		OT_DECL_NODEFAULT(PixmapImagePainter)
		OT_DECL_NOCOPY(PixmapImagePainter)
	public:
		PixmapImagePainter(const QPixmap& _pixmap);
		virtual ~PixmapImagePainter();

		virtual void paintImage(QPainter* _painter, const QRectF& _bounds) const override;

		virtual QSizeF getDefaultImageSize(void) const override { return m_pixmap.size().toSizeF(); };

	private:
		QPixmap m_pixmap;
	};

}