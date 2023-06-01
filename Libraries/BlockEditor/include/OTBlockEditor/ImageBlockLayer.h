//! @file ImageBlockLayer.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockLayer.h"

// Qt header
#include <QtGui/qpixmap.h>

namespace ot {

	class ot::DefaultBlock;

	class BLOCK_EDITOR_API_EXPORT ImageBlockLayer : public BlockLayer {
	public:
		ImageBlockLayer(ot::DefaultBlock* _block);
		virtual ~ImageBlockLayer();

		virtual void paintLayer(const QRectF& _rect, QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) override;

		void setPixmap(const QPixmap& _pixmap) { m_pixmap = _pixmap; };
		const QPixmap& pixmap(void) const { return m_pixmap; };

	private:
		QPixmap m_pixmap;

		ImageBlockLayer() = delete;
		ImageBlockLayer(const ImageBlockLayer&) = delete;
		ImageBlockLayer& operator = (const ImageBlockLayer&) = delete;
	};

}