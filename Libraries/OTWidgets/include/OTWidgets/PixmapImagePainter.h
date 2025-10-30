// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/ImagePainter.h"

// Qt header
#include <QtGui/qpixmap.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PixmapImagePainter : public ImagePainter {
		OT_DECL_NODEFAULT(PixmapImagePainter)
		OT_DECL_NOMOVE(PixmapImagePainter)
	public:
		PixmapImagePainter(const QPixmap& _pixmap);
		PixmapImagePainter(const PixmapImagePainter& _other);
		virtual ~PixmapImagePainter();

		PixmapImagePainter& operator=(const PixmapImagePainter&) = delete;

		virtual void paintImage(QPainter* _painter, const QRect& _bounds, bool _maintainAspectRatio) const override;
		virtual void paintImage(QPainter* _painter, const QRectF& _bounds, bool _maintainAspectRatio) const override;

		virtual ImagePainter* createCopy() const override { return new PixmapImagePainter(*this); };

		virtual QSize getDefaultImageSize() const override { return m_pixmap.size(); };
		virtual QSizeF getDefaultImageSizeF() const override { return m_pixmap.size().toSizeF(); };

	private:
		QPixmap m_pixmap;
	};

}