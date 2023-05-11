//! @file ImageBlockLayer.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockLayer.h"

namespace ot {

	class ot::DefaultBlock;

	class BLOCK_EDITOR_API_EXPORT ImageBlockLayer : public BlockLayer {
	public:
		ImageBlockLayer(ot::DefaultBlock* _block);
		virtual ~ImageBlockLayer();

		virtual void paintLayer(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) override;

	private:

		ImageBlockLayer() = delete;
		ImageBlockLayer(const ImageBlockLayer&) = delete;
		ImageBlockLayer& operator = (const ImageBlockLayer&) = delete;
	};

}