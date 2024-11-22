//! @file GraphicsItemFactory.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsItemFileCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/GraphicsStackItem.h"
#include "OTWidgets/GraphicsItemLoader.h"

ot::GraphicsItemFactory& ot::GraphicsItemFactory::instance(void) {
	static GraphicsItemFactory g_instance;
	return g_instance;
}

ot::GraphicsItem* ot::GraphicsItemFactory::create(const ConstJsonObject& _configObject, bool _isRoot) {
	// Create configuration
	GraphicsItemCfg* cfg = GraphicsItemCfgFactory::create(_configObject);
	if (!cfg) return nullptr;

	// Create item
	return GraphicsItemFactory::itemFromConfig(cfg, _isRoot);
}

ot::GraphicsItem* ot::GraphicsItemFactory::itemFromConfig(const ot::GraphicsItemCfg* _configuration, bool _isRoot) {
	OTAssertNullptr(_configuration);

	// If the configuration is a file we need to import the actual configuration for this item.
	if (_configuration->getFactoryKey() == OT_FactoryKey_GraphicsFileItem) {
		// Get config and file sub path
		const ot::GraphicsItemFileCfg* actualConfig = dynamic_cast<const ot::GraphicsItemFileCfg*>(_configuration);
		OTAssertNullptr(actualConfig);
		std::string configSubPath = actualConfig->getFile();

		// Import new config
		ot::GraphicsItemCfg* newConfiguration = GraphicsItemLoader::instance().createConfiguration(QString::fromStdString(configSubPath));
		if (!_configuration) {
			return nullptr;
		}

		// Copy config data
		actualConfig->copyConfigDataToItem(newConfiguration);
		_configuration = newConfiguration;
	}

	// Create item
	GraphicsItem* itm = GraphicsItemFactory::instance().createFromKey(_configuration->getFactoryKey());
	if (!itm) return nullptr;
	if (!itm->setupFromConfig(_configuration)) {
		OT_LOG_EA("Setup from configuration failed");
		delete itm;
		return nullptr;
	}

	if (_isRoot) itm->finalizeGraphicsItem();

	return itm;
}
