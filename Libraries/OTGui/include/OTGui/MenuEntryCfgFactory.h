//! @file MenuEntryCfgFactory.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/FactoryTemplate.h"
#include "OTGui/MenuEntryCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT MenuEntryCfgFactory : public FactoryTemplate<MenuEntryCfg> {
		OT_DECL_NOCOPY(MenuEntryCfgFactory)
	public:
		static MenuEntryCfgFactory& instance(void);

		static MenuEntryCfg* create(const ConstJsonObject& _jsonObject);

	private:
		MenuEntryCfgFactory();
		~MenuEntryCfgFactory();
	};

	template <class T>
	class OT_GUI_API_EXPORT MenuEntryCfgFactoryRegistrar : public FactoryRegistrarTemplate<MenuEntryCfgFactory, T> {
	public:
		MenuEntryCfgFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<MenuEntryCfgFactory, T>(_key) {};
	};

}
