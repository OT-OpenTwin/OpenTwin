//! @file PropertyInputInt.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class SpinBox;
	class PropertyInt;

	class PropertyInputInt : public PropertyInput {
		OT_DECL_NOCOPY(PropertyInputInt)
	public:
		PropertyInputInt(const PropertyInt* _property);
		virtual ~PropertyInputInt();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberName, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;

	private:
		SpinBox* m_spinBox;
	};

}