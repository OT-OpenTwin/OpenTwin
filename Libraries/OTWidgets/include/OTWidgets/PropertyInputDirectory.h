//! @file PropertyInputDirectory.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class LineEdit;
	class PushButton;

	class OT_WIDGETS_API_EXPORT PropertyInputDirectory : public PropertyInput {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInputDirectory)
	public:
		PropertyInputDirectory();
		virtual ~PropertyInputDirectory();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual const QWidget* getQWidget(void) const override;
		virtual Property* createPropertyConfiguration(void) const override;
		virtual bool setupFromConfiguration(const Property* _configuration) override;
		virtual void focusPropertyInput(void) override;

		void setCurrentDirectory(const QString& _dir);
		QString currentDirectory(void) const;

	private Q_SLOTS:
		void slotFind(void);
		void slotChanged(void);

	private:
		QWidget* m_root;
		LineEdit* m_edit;
		PushButton* m_find;
		QString m_text;
	};

}