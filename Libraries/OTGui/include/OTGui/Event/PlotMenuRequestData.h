// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Event/MenuRequestData.h"

namespace ot {

	class OT_GUI_API_EXPORT PlotMenuRequestData : public ot::MenuRequestData
	{
		OT_DECL_NOCOPY(PlotMenuRequestData)
		OT_DECL_NOMOVE(PlotMenuRequestData)
	public:
		PlotMenuRequestData();
		PlotMenuRequestData(const BasicEntityInformation& _basicEntityInformation);
		PlotMenuRequestData(BasicEntityInformation&& _basicEntityInformation) noexcept;
		virtual ~PlotMenuRequestData();

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		static constexpr const char* className() noexcept { return "PlotMenuRequestData"; };
		virtual std::string getClassName() const { return PlotMenuRequestData::className(); };

	};
}