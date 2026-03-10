// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Event/ContextRequestData.h"

namespace ot {

	
	class OT_GUI_API_EXPORT NavigationContextRequestData : public ContextRequestData
	{
		OT_DECL_DEFCOPY(NavigationContextRequestData)
		OT_DECL_DEFMOVE(NavigationContextRequestData)
	public:
		NavigationContextRequestData();
		NavigationContextRequestData(const BasicEntityInformation& _basicEntityInformation);
		NavigationContextRequestData(BasicEntityInformation&& _basicEntityInformation) noexcept;
		~NavigationContextRequestData() = default;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		static constexpr const char* className() noexcept { return "NavigationContextRequestData"; };
		virtual std::string getClassName() const { return NavigationContextRequestData::className(); };
	};


}