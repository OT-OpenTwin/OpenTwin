//! @file LayerFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/DefaultBlock.h"
#include "OTBlockEditor/PainterFactory.h"
#include "OTBlockEditor/LayerFactory.h"
#include "OTBlockEditor/TextBlockLayer.h"
#include "OTBlockEditor/ImageBlockLayer.h"
#include "OTBlockEditor/RectangularBlockLayer.h"
#include "OTBlockEditorAPI/BlockLayers.h"
#include "OTQtWrapper/TypeConversion.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"

ot::BlockLayer* ot::LayerFactory::blockLayerFromConfig(ot::DefaultBlock* _block, ot::BlockLayerConfiguration* _config) {
	OTAssertNullptr(_block);
	OTAssertNullptr(_config);

	ot::BlockLayer* layer = nullptr;

	// Specific block type
	if (_config->layerType() == OT_IMAGEBLOCKLAYERCONFIGURATION_TYPE) {
		layer = blockLayerFromConfig(_block, dynamic_cast<ot::ImageBlockLayerConfiguration*>(_config));
	}
	else if (_config->layerType() == OT_TEXTBLOCKLAYERCONFIGURATION_TYPE) {
		layer = blockLayerFromConfig(_block, dynamic_cast<ot::TextBlockLayerConfiguration*>(_config));
	}
	else if (_config->layerType() == OT_RECTANGLEBLOCKLAYERCONFIGURATION_TYPE) {
		layer = blockLayerFromConfig(_block, dynamic_cast<ot::RectangleBlockLayerConfiguration*>(_config));
	}
	else {
		OT_LOG_EAS("Unknown block layer configuration type (type = \"" + _config->layerType() + "\")");
	}

	if (layer == nullptr) return layer;

	// General settings
	layer->setMargins(_config->margins());
	layer->setHeightLimit(_config->heightLimits());
	layer->setWidthLimit(_config->widthLimits());
	layer->setLayerOrientation(_config->orientation());

	return layer;
}

ot::ImageBlockLayer* ot::LayerFactory::blockLayerFromConfig(ot::DefaultBlock* _block, ot::ImageBlockLayerConfiguration* _config) {
	OTAssertNullptr(_block);
	OTAssertNullptr(_config);

	otAssert(0, "ot implemented yet");
	return nullptr;
}

ot::TextBlockLayer* ot::LayerFactory::blockLayerFromConfig(ot::DefaultBlock* _block, ot::TextBlockLayerConfiguration* _config) {
	OTAssertNullptr(_block);
	OTAssertNullptr(_config);

	ot::TextBlockLayer* newLayer = new ot::TextBlockLayer(_block);
	newLayer->setTextColor(colorToQColor(_config->textColor()));
	newLayer->setText(QString::fromStdString(_config->text()));
	newLayer->setTextFont(ot::fontToQFont(_config->textFont()));

	return newLayer;
}

ot::RectangularBlockLayer* ot::LayerFactory::blockLayerFromConfig(ot::DefaultBlock* _block, ot::RectangleBlockLayerConfiguration* _config) {
	OTAssertNullptr(_block);
	OTAssertNullptr(_config);
	
	ot::RectangularBlockLayer* newLayer = new ot::RectangularBlockLayer(_block);
	newLayer->setBorder(colorToQColor(_config->borderColor()), _config->borderWidth());
	if (_config->backgroundPainter()) newLayer->setBrush(PainterFactory::brushFromPainter(_config->backgroundPainter()));
	
	return newLayer;
}