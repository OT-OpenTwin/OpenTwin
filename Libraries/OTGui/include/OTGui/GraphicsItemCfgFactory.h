//! @file GraphicsItemCfgFactory.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/FactoryTemplate.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/GraphicsItemCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsItemCfgFactory : public FactoryTemplate<std::string, GraphicsItemCfg> {
	public:
		static GraphicsItemCfgFactory& instance(void);

		GraphicsItemCfg* create(const ConstJsonObject& _object);

	private:
		GraphicsItemCfgFactory() {};
		virtual ~GraphicsItemCfgFactory() {};
	};

	template <class T>
	class OT_GUI_API_EXPORT GraphicsItemCfgFactoryRegistrar : public FactoryRegistrarTemplate<std::string, GraphicsItemCfgFactory, T> {
	public:
		GraphicsItemCfgFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<std::string, GraphicsItemCfgFactory, T>(_key) {};
	};

}
