//! @file GraphicsFactory.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/FactoryTemplate.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	class GraphicsItemCfg;

	//! @class GraphicsItemFactory.
	//! @brief The GraphicsItemFactory is used to create graphics items from their factory name or from a GraphicsItemCfg.
	//! @note Note that items that register at this factory must register with the same key as their configuration equivalents.
	class OT_WIDGETS_API_EXPORT GraphicsItemFactory : public FactoryTemplate<std::string, GraphicsItem> {
	public:
		static GraphicsItemFactory& instance(void);

		//! @brief Will create the configuration from the provided json object and then create the item using the created configuration.
		//! @param _configObject The JSON object containing the GraphicsItemCfg.
		ot::GraphicsItem* create(const ConstJsonObject& _configObject, bool _isRoot = false);

		//! @brief Will create a graphics item from the provided configuration.
		//! The graphics item will be created according to the graphics item cfg name.
		//! @param _config The graphics item configuration.
		//! @param _isRoot If true, the item will be added to a stack with a highlight item on top. The highlight item will be set to the root item.
		ot::GraphicsItem* itemFromConfig(ot::GraphicsItemCfg* _config, bool _isRoot = false);

	private:
		GraphicsItemFactory() {};
		~GraphicsItemFactory() {};
	};

	//! @class GraphicsItemFactoryRegistrar.
	//! @brief The GraphicsItemFactoryRegistrar is used to register a GraphicsItem constructor at the GraphicsItemFactory.
	//! @note Note that the key used to register must be equal to the configraion equivalent (see GraphicsItemCfg).
	template<class T> class GraphicsItemFactoryRegistrar : public FactoryRegistrarTemplate<std::string, GraphicsItemFactory, T> {
	public:
		GraphicsItemFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<std::string, GraphicsItemFactory, T>(_key) {};
	};

}
