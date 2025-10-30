// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/FactoryTemplate.h"
#include "OTGui/WidgetBaseCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT WidgetBaseCfgFactory : public FactoryTemplate<WidgetBaseCfg> {
	public:
		static WidgetBaseCfgFactory& instance(void);

		static WidgetBaseCfg* create(const ConstJsonObject& _object);

	private:
		WidgetBaseCfgFactory() {};
		~WidgetBaseCfgFactory() {};
	};

	template <class T>
	class OT_GUI_API_EXPORT WidgetBaseCfgFactoryRegistrar : public FactoryRegistrarTemplate<WidgetBaseCfgFactory, T> {
	public:
		WidgetBaseCfgFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<WidgetBaseCfgFactory, T>(_key) {};

	private:

	};
}