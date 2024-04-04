//! @file PropertyInputColor.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class ColorPickButton;
	class PropertyColor;

	class OT_WIDGETS_API_EXPORT PropertyInputColor : public PropertyInput {
		OT_DECL_NOCOPY(PropertyInputColor)
	public:
		PropertyInputColor(const PropertyColor* _property);
		virtual ~PropertyInputColor();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual Property* createPropertyConfiguration(void) const override;

		void setColor(const Color& _color);
		void setColor(const QColor& _color);
		Color getOTColor(void) const;
		QColor getColor(void) const;

	private:
		ColorPickButton* m_colorBtn;
	};

}