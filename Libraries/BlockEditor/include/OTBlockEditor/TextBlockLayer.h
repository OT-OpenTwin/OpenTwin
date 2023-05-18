//! @file TextBlockLayer.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockLayer.h"

namespace ot {

	class ot::DefaultBlock;

	class BLOCK_EDITOR_API_EXPORT TextBlockLayer : public BlockLayer {
	public:
		TextBlockLayer(ot::DefaultBlock* _block);
		virtual ~TextBlockLayer();

		virtual void paintLayer(const QRectF& _rect, QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) override;

	private:

		TextBlockLayer() = delete;
		TextBlockLayer(const TextBlockLayer&) = delete;
		TextBlockLayer& operator = (const TextBlockLayer&) = delete;
	};

}