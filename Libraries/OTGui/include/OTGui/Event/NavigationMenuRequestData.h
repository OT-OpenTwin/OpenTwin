// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Event/MenuRequestData.h"

namespace ot {

	
	class OT_GUI_API_EXPORT NavigationMenuRequestData : public MenuRequestData
	{
		OT_DECL_DEFCOPY(NavigationMenuRequestData)
		OT_DECL_DEFMOVE(NavigationMenuRequestData)
	public:
		NavigationMenuRequestData();
		NavigationMenuRequestData(const BasicEntityInformation& _basicEntityInformation);
		NavigationMenuRequestData(BasicEntityInformation&& _basicEntityInformation) noexcept;
		~NavigationMenuRequestData() = default;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		static constexpr const char* className() noexcept { return "NavigationMenuRequestData"; };
		virtual std::string getClassName() const { return NavigationMenuRequestData::className(); };
	};


}