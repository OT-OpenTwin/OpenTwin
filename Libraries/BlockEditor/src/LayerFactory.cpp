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
#include "OTBlockEditor/BlockConnectorManagerFactory.h"
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
		layer = ot::LayerFactory::intern::blockLayerFromConfig(_block, dynamic_cast<ot::ImageBlockLayerConfiguration*>(_config));
	}
	else if (_config->layerType() == OT_TEXTBLOCKLAYERCONFIGURATION_TYPE) {
		layer = ot::LayerFactory::intern::blockLayerFromConfig(_block, dynamic_cast<ot::TextBlockLayerConfiguration*>(_config));
	}
	else if (_config->layerType() == OT_RECTANGLEBLOCKLAYERCONFIGURATION_TYPE) {
		layer = ot::LayerFactory::intern::blockLayerFromConfig(_block, dynamic_cast<ot::RectangleBlockLayerConfiguration*>(_config));
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

	if (_config->getConnectorManager()) {
		layer->setConnectorManger(BlockConnectorManagerFactory::connectorManagerFromConfig(layer, _config->getConnectorManager()));
	}

	return layer;
}

ot::ImageBlockLayer* ot::LayerFactory::intern::blockLayerFromConfig(ot::DefaultBlock* _block, ot::ImageBlockLayerConfiguration* _config) {
	OTAssertNullptr(_block);
	OTAssertNullptr(_config);

	ot::ImageBlockLayer* newLayer = new ot::ImageBlockLayer(_block);
	otAssert(0, "Image layer NOT using image loader from uiAPI yet!");
	QPixmap pix(QString::fromStdString(_config->imagePath()));
	newLayer->setPixmap(pix);

	return newLayer;
}

ot::TextBlockLayer* ot::LayerFactory::intern::blockLayerFromConfig(ot::DefaultBlock* _block, ot::TextBlockLayerConfiguration* _config) {
	OTAssertNullptr(_block);
	OTAssertNullptr(_config);

	ot::TextBlockLayer* newLayer = new ot::TextBlockLayer(_block);
	newLayer->setTextColor(colorToQColor(_config->textColor()));
	newLayer->setText(QString::fromStdString(_config->text()));
	newLayer->setTextFont(ot::fontToQFont(_config->textFont()));

	return newLayer;
}

ot::RectangularBlockLayer* ot::LayerFactory::intern::blockLayerFromConfig(ot::DefaultBlock* _block, ot::RectangleBlockLayerConfiguration* _config) {
	OTAssertNullptr(_block);
	OTAssertNullptr(_config);
	
	ot::RectangularBlockLayer* newLayer = new ot::RectangularBlockLayer(_block);
	newLayer->setBorder(colorToQColor(_config->borderColor()), _config->borderWidth());
	if (_config->backgroundPainter()) newLayer->setBrush(PainterFactory::brushFromPainter(_config->backgroundPainter()));
	
	return newLayer;
}