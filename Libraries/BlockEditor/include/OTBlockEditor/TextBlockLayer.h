//! @file TextBlockLayer.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockLayer.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtGui/qfont.h>
#include <QtGui/qcolor.h>

namespace ot {

	class ot::DefaultBlock;

	class BLOCK_EDITOR_API_EXPORT TextBlockLayer : public BlockLayer {
	public:
		TextBlockLayer(ot::DefaultBlock* _block);
		virtual ~TextBlockLayer();

		virtual void paintLayer(const QRectF& _rect, QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) override;

		void setText(const QString& _text) { m_text = _text; };
		const QString& text(void) const { return m_text; };

		void setTextFont(const QFont& _font) { m_textFont = _font; };
		const QFont& textFont(void) const { return m_textFont; };

		void setTextColor(const QColor& _color) { m_textColor = _color; };
		const QColor& textColor(void) const { return m_textColor; };

	private:
		QString m_text;
		QFont m_textFont;
		QColor m_textColor;

		TextBlockLayer() = delete;
		TextBlockLayer(const TextBlockLayer&) = delete;
		TextBlockLayer& operator = (const TextBlockLayer&) = delete;
	};

}