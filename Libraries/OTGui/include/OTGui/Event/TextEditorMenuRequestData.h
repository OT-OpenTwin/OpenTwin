// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Event/MenuRequestData.h"

namespace ot {
	
	class OT_GUI_API_EXPORT TextEditorMenuRequestData : public ot::MenuRequestData
	{
		OT_DECL_DEFCOPY(TextEditorMenuRequestData)
		OT_DECL_DEFMOVE(TextEditorMenuRequestData)
	public:
		TextEditorMenuRequestData();
		virtual ~TextEditorMenuRequestData();

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		static constexpr const char* className() noexcept { return "TextEditorMenuRequestData"; };
		virtual std::string getClassName() const { return TextEditorMenuRequestData::className(); };

	};
}