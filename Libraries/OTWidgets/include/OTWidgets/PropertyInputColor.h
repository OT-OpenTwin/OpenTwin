//! @file PropertyInputColor.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTWidgets/PropertyInput.h"

// Qt header
#include <QtGui/qcolor.h>

namespace ot {

	class ColorPickButton;

	class OT_WIDGETS_API_EXPORT PropertyInputColor : public PropertyInput {
		OT_DECL_NOCOPY(PropertyInputColor)
	public:
		PropertyInputColor();
		virtual ~PropertyInputColor();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual const QWidget* getQWidget(void) const override;
		virtual Property* createPropertyConfiguration(void) const override;
		virtual bool setupFromConfiguration(const Property* _configuration) override;
		virtual void focusPropertyInput(void) override;

		void setColor(const Color& _color);
		void setColor(const QColor& _color);
		Color getOTColor(void) const;
		QColor getColor(void) const;

	private:
		ColorPickButton* m_colorBtn;
	};

}