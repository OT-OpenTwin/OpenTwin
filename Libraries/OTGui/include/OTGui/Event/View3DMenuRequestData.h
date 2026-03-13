// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Event/MenuRequestData.h"

namespace ot {

	class OT_GUI_API_EXPORT View3DMenuRequestData : public ot::MenuRequestData
	{
		OT_DECL_NOCOPY(View3DMenuRequestData)
		OT_DECL_NOMOVE(View3DMenuRequestData)
	public:
		View3DMenuRequestData();
		View3DMenuRequestData(const BasicEntityInformation& _basicEntityInformation);
		View3DMenuRequestData(BasicEntityInformation&& _basicEntityInformation) noexcept;
		virtual ~View3DMenuRequestData();

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		static constexpr const char* className() noexcept { return "View3DMenuRequestData"; };
		virtual std::string getClassName() const { return View3DMenuRequestData::className(); };

	};
}