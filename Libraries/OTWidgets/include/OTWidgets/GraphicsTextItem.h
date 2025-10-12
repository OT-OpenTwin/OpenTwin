//! @file GraphicsTextItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PenCfg.h"
#include "OTWidgets/CustomGraphicsItem.h"

// Qt header
#include <QtCore/qstring.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsTextItem : public ot::CustomGraphicsItem {
	public:
		GraphicsTextItem();
		virtual ~GraphicsTextItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::GraphicsItem

		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

		virtual QMarginsF getOutlineMargins(void) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::CustomGraphicsItem

		virtual QSizeF getPreferredGraphicsItemSize(void) const override;

		virtual void finalizeGraphicsItem(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setText(const char* _text) { this->setText(std::string(_text)); };
		void setText(const std::string& _text);
		void setText(const QString& _text) { this->setText(_text.toStdString()); };
		const std::string& getText(void) const;

		void setFont(const Font& _font);
		const Font& getFont(void) const;

		void setTextColor(const Color& _color);

		//! @brief Sets the text painter.
		//! The item takes ownership of the painter.
		void setTextPainter(Painter2D* _painter);

		const Painter2D* getTextPainter(void) const;

		void setTextIsReference(bool _isReference);
		bool getTextIsReference(void) const;

	protected:

		//! @brief Paint the item inside the provided rect
		virtual void paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) override;

	};

}