// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/FactoryTemplate.h"
#include "OTGui/Painter2D.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class OT_GUI_API_EXPORT Painter2DFactory : public FactoryTemplate<Painter2D> {
	public:
		static Painter2DFactory& instance(void);
		
		static Painter2D* create(const ConstJsonObject& _jsonObject);

	private:
		Painter2DFactory() {};
		virtual ~Painter2DFactory() {};
	};

	template <class T>
	class OT_GUI_API_EXPORT Painter2DFactoryRegistrar : public FactoryRegistrarTemplate<Painter2DFactory, T> {
	public:
		Painter2DFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<Painter2DFactory, T>(_key) {};
	};
}
