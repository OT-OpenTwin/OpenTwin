//! @file PropertyInputDouble.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class DoubleSpinBox;
	class PropertyDouble;

	class OT_WIDGETS_API_EXPORT PropertyInputDouble : public PropertyInput {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInputDouble)
	public:
		PropertyInputDouble(const PropertyDouble* _property);
		virtual ~PropertyInputDouble();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual Property* createPropertyConfiguration(void) const override;

		DoubleSpinBox* getSpinBox(void) const { return m_spinBox; };

	private Q_SLOTS:
		void lclValueChanged(int);

	private:
		DoubleSpinBox* m_spinBox;
	};

}
