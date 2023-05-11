//! @file RectangularBlockLayer.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockLayer.h"

// Qt header
#include <QtGui/qcolor.h>
#include <QtGui/qbrush.h>

namespace ot {

	class ot::DefaultBlock;

	class BLOCK_EDITOR_API_EXPORT RectangularBlockLayer : public BlockLayer {
	public:
		RectangularBlockLayer(ot::DefaultBlock* _block);
		virtual ~RectangularBlockLayer();

		virtual void paintLayer(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) override;

	private:
		int m_cornerRadius;
		int m_borderWidth;
		QColor m_borderColor;
		QBrush m_brush;

		RectangularBlockLayer() = delete;
		RectangularBlockLayer(const RectangularBlockLayer&) = delete;
		RectangularBlockLayer& operator = (const RectangularBlockLayer&) = delete;
	};

}