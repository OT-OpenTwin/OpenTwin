//! @file PropertyInputStringList.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class ComboButton;

	class OT_WIDGETS_API_EXPORT PropertyInputStringList : public PropertyInput {
		OT_DECL_NOCOPY(PropertyInputStringList)
	public:
		PropertyInputStringList();
		virtual ~PropertyInputStringList();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual const QWidget* getQWidget(void) const override;
		virtual Property* createPropertyConfiguration(void) const override;
		virtual bool setupFromConfiguration(const Property* _configuration) override;
		virtual void focusPropertyInput(void) override;

		void setCurrentText(const QString& _text);
		QString getCurrentText(void) const;
		QStringList getPossibleSelection(void) const;

	private:
		ComboButton* m_comboButton;
	};

}