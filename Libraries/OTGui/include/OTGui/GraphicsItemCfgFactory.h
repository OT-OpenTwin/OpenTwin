// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/FactoryTemplate.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/GraphicsItemCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsItemCfgFactory : public FactoryTemplate<GraphicsItemCfg> {
	public:
		static GraphicsItemCfgFactory& instance(void);

		static GraphicsItemCfg* create(const ConstJsonObject& _object);

	private:
		GraphicsItemCfgFactory() {};
		virtual ~GraphicsItemCfgFactory() {};
	};

	template <class T>
	class OT_GUI_API_EXPORT GraphicsItemCfgFactoryRegistrar : public FactoryRegistrarTemplate<GraphicsItemCfgFactory, T> {
	public:
		GraphicsItemCfgFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<GraphicsItemCfgFactory, T>(_key) {};
	};

}
