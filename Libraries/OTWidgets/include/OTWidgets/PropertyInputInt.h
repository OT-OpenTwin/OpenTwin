//! @file PropertyInputInt.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class SpinBox;
	class LineEdit;

	class OT_WIDGETS_API_EXPORT PropertyInputInt : public PropertyInput {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInputInt)
	public:
		PropertyInputInt();
		virtual ~PropertyInputInt();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual Property* createPropertyConfiguration(void) const override;
		virtual bool setupFromConfiguration(const Property* _configuration) override;
		virtual void focusPropertyInput(void) override;

		void setValue(int _value);
		int getValue(void) const;

		bool hasInputError(void) const;

	private Q_SLOTS:
		void lclValueChanged(int);
		void lclTextChanged(void);
		void lclEditingFinishedChanged(void);

	private:
		int m_min;
		int m_max;
		SpinBox* m_spinBox;
		LineEdit* m_lineEdit;
	};

}
