//! @file RectangularBlockLayer.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockLayer.h"
#include "OTGui/Border.h"
#include "OpenTwinCore/Color.h"

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

		void setBorder(const ot::Border& _border);
		void setBorder(const QColor& _color, int _borderWidth);

		void setBorderWidth(int _borderWidth) { m_borderWidth = _borderWidth; };
		int borderWidth(void) const { return m_borderWidth; };

		void setBorderColor(const QColor& _borderColor) { m_borderColor = _borderColor; };
		const QColor& borderColor(void) const { return m_borderColor; };

		void setBrush(const QBrush& _brush) { m_brush = _brush; };
		const QBrush& brush(void) { return m_brush; };

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