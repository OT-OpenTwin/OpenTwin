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

	class OT_WIDGETS_API_EXPORT PropertyInputInt : public PropertyInput {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInputInt)
	public:
		PropertyInputInt(const PropertyInt* _property);
		virtual ~PropertyInputInt();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual Property* createPropertyConfiguration(void) const override;

		SpinBox* getSpinBox(void) const { return m_spinBox; };

	private slots:
		void lclValueChanged(int);

	private:
		SpinBox* m_spinBox;
	};

}