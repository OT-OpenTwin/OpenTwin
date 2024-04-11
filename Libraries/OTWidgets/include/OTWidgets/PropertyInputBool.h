//! @file PropertyInputBool.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class CheckBox;

	class OT_WIDGETS_API_EXPORT PropertyInputBool : public PropertyInput {
		OT_DECL_NOCOPY(PropertyInputBool)
	public:
		PropertyInputBool();
		virtual ~PropertyInputBool();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual Property* createPropertyConfiguration(void) const override;
		virtual bool setupFromConfiguration(const Property* _configuration) override;

		void setChecked(bool _isChecked);
		bool isChecked(void) const;

	private:
		CheckBox* m_checkBox;
	};

}