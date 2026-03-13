// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Event/MenuRequestData.h"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsMenuRequestData : public ot::MenuRequestData
	{
		OT_DECL_NOCOPY(GraphicsMenuRequestData)
		OT_DECL_NOMOVE(GraphicsMenuRequestData)
	public:
		GraphicsMenuRequestData();
		GraphicsMenuRequestData(const BasicEntityInformation& _basicEntityInformation);
		GraphicsMenuRequestData(BasicEntityInformation&& _basicEntityInformation) noexcept;
		virtual ~GraphicsMenuRequestData();

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		static constexpr const char* className() noexcept { return "GraphicsMenuRequestData"; };
		virtual std::string getClassName() const { return GraphicsMenuRequestData::className(); };

	};
}