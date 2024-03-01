//! @file PropertyInputString.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyInput.h"

// Qt header
#include <QtCore/qstring.h>

namespace ot {

	class LineEdit;
	class PropertyString;

	class PropertyInputString : public PropertyInput {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInputString)
	public:
		PropertyInputString(const PropertyString* _property);
		virtual ~PropertyInputString();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;

	private Q_SLOTS:
		void lclValueChanged(void);

	private:
		LineEdit* m_lineEdit;
		QString m_text;
	};

}