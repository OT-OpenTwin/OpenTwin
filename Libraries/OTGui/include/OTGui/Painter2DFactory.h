//! @file Painter2DFactory.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/FactoryTemplate.h"
#include "OTGui/Painter2D.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class OT_GUI_API_EXPORT Painter2DFactory : public FactoryTemplate<std::string, Painter2D> {
	public:
		static Painter2DFactory& instance(void);
		
		Painter2D* create(const ConstJsonObject& _jsonObject);

	private:
		Painter2DFactory() {};
		virtual ~Painter2DFactory() {};
	};

	template <class T>
	class OT_GUI_API_EXPORT Painter2DFactoryRegistrar : public FactoryRegistrarTemplate<std::string, Painter2DFactory, T> {
	public:
		Painter2DFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<std::string, Painter2DFactory, T>(_key) {};
	};
}
