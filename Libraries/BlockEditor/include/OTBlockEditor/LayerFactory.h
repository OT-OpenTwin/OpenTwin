//! @file LayerFactory.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"

namespace ot {

	class DefaultBlock;

	class BlockLayer;
	class ImageBlockLayer;
	class TextBlockLayer;
	class RectangularBlockLayer;

	class BlockLayerConfiguration;
	class ImageBlockLayerConfiguration;
	class TextBlockLayerConfiguration;
	class RectangleBlockLayerConfiguration;

	namespace LayerFactory {

		BLOCK_EDITOR_API_EXPORT ot::BlockLayer* blockLayerFromConfig(ot::DefaultBlock* _block, ot::BlockLayerConfiguration* _config);
		BLOCK_EDITOR_API_EXPORT ot::ImageBlockLayer* blockLayerFromConfig(ot::DefaultBlock* _block, ot::ImageBlockLayerConfiguration* _config);
		BLOCK_EDITOR_API_EXPORT ot::TextBlockLayer* blockLayerFromConfig(ot::DefaultBlock* _block, ot::TextBlockLayerConfiguration* _config);
		BLOCK_EDITOR_API_EXPORT ot::RectangularBlockLayer* blockLayerFromConfig(ot::DefaultBlock* _block, ot::RectangleBlockLayerConfiguration* _config);

	}
}