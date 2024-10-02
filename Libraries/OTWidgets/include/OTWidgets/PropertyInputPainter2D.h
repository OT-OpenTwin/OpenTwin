//! @file PropertyInputPainter2D.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class Painter2D;
	class Painter2DEditButton;

	class OT_WIDGETS_API_EXPORT PropertyInputPainter2D : public PropertyInput {
		OT_DECL_NOCOPY(PropertyInputPainter2D)
	public:
		PropertyInputPainter2D();
		virtual ~PropertyInputPainter2D();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual Property* createPropertyConfiguration(void) const override;
		virtual bool setupFromConfiguration(const Property* _configuration) override;
		virtual void focusPropertyInput(void) override;

		const Painter2D* getPainter(void) const;
		Painter2DEditButton* getButton(void) const { return m_button; };

	private:
		Painter2DEditButton* m_button;
	};

}