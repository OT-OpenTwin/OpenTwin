//! @file SvgImagePainter.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/ImagePainter.h"

class QSvgRenderer;

namespace ot {

	class OT_WIDGETS_API_EXPORT SvgImagePainter : public ImagePainter {
		OT_DECL_NODEFAULT(SvgImagePainter)
		OT_DECL_NOCOPY(SvgImagePainter)
	public:
		SvgImagePainter(QSvgRenderer* _renderer);
		virtual ~SvgImagePainter();

		virtual void paintImage(QPainter* _painter, const QRectF& _bounds) const override;

		virtual QSizeF getDefaultImageSize(void) const override;

	private:
		QSvgRenderer* m_svgRenderer;
	};

}