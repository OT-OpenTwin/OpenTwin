//! @file PropertyInputFilePath.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PropertyFilePath.h"
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class LineEdit;

	class OT_WIDGETS_API_EXPORT PropertyInputFilePath : public PropertyInput {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInputFilePath)
	public:
		PropertyInputFilePath(const PropertyFilePath* _property);
		virtual ~PropertyInputFilePath();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;

	private Q_SLOTS:
		void slotFind(void);
		void slotChanged(void);

	private:
		PropertyFilePath::BrowseMode m_mode;
		QWidget* m_root;
		LineEdit* m_edit;
		QString m_filter;
		QString m_text;
	};

}