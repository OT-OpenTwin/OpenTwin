//! @file PropertyInputDouble.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class LineEdit;
	class DoubleSpinBox;

	class OT_WIDGETS_API_EXPORT PropertyInputDouble : public PropertyInput {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInputDouble)
	public:
		PropertyInputDouble();
		virtual ~PropertyInputDouble();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual Property* createPropertyConfiguration(void) const override;
		virtual bool setupFromConfiguration(const Property* _configuration) override;

		void setValue(double _value);
		double getValue(void) const;

		bool hasInputError(void) const;

	private Q_SLOTS:
		void lclValueChanged(int);
		void lclTextChanged(void);
		void lclEditingFinishedChanged(void);

	private:
		LineEdit* m_lineEdit;
		DoubleSpinBox* m_spinBox;
	};

}
