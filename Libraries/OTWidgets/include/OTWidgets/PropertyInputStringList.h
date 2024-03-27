//! @file PropertyInputStringList.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class ComboButton;
	class PropertyStringList;

	class OT_WIDGETS_API_EXPORT PropertyInputStringList : public PropertyInput {
		OT_DECL_NOCOPY(PropertyInputStringList)
	public:
		PropertyInputStringList(const PropertyStringList* _property);
		virtual ~PropertyInputStringList();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual Property* createPropertyConfiguration(void) const override;

	private:
		ComboButton* m_comboButton;
	};

}