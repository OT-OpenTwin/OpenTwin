// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Event/MenuRequestData.h"

namespace ot {

	class OT_GUI_API_EXPORT TableMenuRequestData : public ot::MenuRequestData
	{
		OT_DECL_NOCOPY(TableMenuRequestData)
		OT_DECL_NOMOVE(TableMenuRequestData)
	public:
		TableMenuRequestData();
		TableMenuRequestData(const BasicEntityInformation& _basicEntityInformation);
		TableMenuRequestData(BasicEntityInformation&& _basicEntityInformation) noexcept;
		virtual ~TableMenuRequestData();

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		static constexpr const char* className() noexcept { return "TableMenuRequestData"; };
		virtual std::string getClassName() const { return TableMenuRequestData::className(); };

	};
}