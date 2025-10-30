// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/PropertyPath.h"
#include "OTWidgets/PropertyInput.h"

namespace ot {

	class PathBrowseEdit;

	class OT_WIDGETS_API_EXPORT PropertyInputPath : public PropertyInput {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInputPath)
	public:
		PropertyInputPath();
		virtual ~PropertyInputPath();

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
		PathBrowseEdit* m_path;
	};

}