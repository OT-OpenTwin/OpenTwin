// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/FactoryTemplate.h"
#include "OTGui/GraphicsItemCfg.h"

// std header
#include <string>

class GraphicsItemDesignerItemBase;

class WrappedItemFactory : public ot::FactoryTemplate<GraphicsItemDesignerItemBase> {
public:
	static WrappedItemFactory& instance(void);

	static GraphicsItemDesignerItemBase* createFromConfig(const ot::GraphicsItemCfg* _config);

};

template <class T>
class WrappedItemFactoryRegistrar : public ot::FactoryRegistrarTemplate<WrappedItemFactory, T> {
public:
	WrappedItemFactoryRegistrar(const std::string& _key) : ot::FactoryRegistrarTemplate<WrappedItemFactory, T>(_key) {};
};