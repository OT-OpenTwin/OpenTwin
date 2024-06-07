//! @file WrappedItemFactory.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/FactoryTemplate.h"
#include "OTGui/GraphicsItemCfg.h"

// std header
#include <string>

class GraphicsItemDesignerItemBase;

class WrappedItemFactory : public ot::FactoryTemplate<std::string, GraphicsItemDesignerItemBase> {
public:
	static WrappedItemFactory& instance(void);

	GraphicsItemDesignerItemBase* createFromConfig(const ot::GraphicsItemCfg* _config);

};

template <class T>
class WrappedItemFactoryRegistrar : public ot::FactoryRegistrarTemplate<std::string, WrappedItemFactory, T> {
public:
	WrappedItemFactoryRegistrar(const std::string& _key) : ot::FactoryRegistrarTemplate<std::string, WrappedItemFactory, T>(_key) {};
};