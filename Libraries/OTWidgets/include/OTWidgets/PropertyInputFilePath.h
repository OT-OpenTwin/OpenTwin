//! @file PropertyInputFilePath.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PropertyFilePath.h"
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class FilePathEdit;

	class OT_WIDGETS_API_EXPORT PropertyInputFilePath : public PropertyInput {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInputFilePath)
	public:
		PropertyInputFilePath();
		virtual ~PropertyInputFilePath();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual const QWidget* getQWidget(void) const override;
		virtual Property* createPropertyConfiguration(void) const override;
		virtual bool setupFromConfiguration(const Property* _configuration) override;
		virtual void focusPropertyInput(void) override;

		void setCurrentFile(const QString& _file);
		QString currentFile(void) const;

	private:
		PropertyFilePath::BrowseMode m_mode;
		FilePathEdit* m_path;
		QString m_filter;
		QString m_text;
	};

}