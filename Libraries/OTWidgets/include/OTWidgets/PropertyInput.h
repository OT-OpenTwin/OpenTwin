//! @file PropertyInput.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Property.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

namespace ot {

	class Property;

	class OT_WIDGETS_API_EXPORT PropertyInput : public QObject, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInput)
	public:
		PropertyInput();
		virtual ~PropertyInput() {};

		//! @brief Add the current value to the provided JSON object
		//! @param _object Target JSON object
		//! @param _memberNameValue Value member name
		//! @param _allocator JSON allocator
		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) = 0;

		//! @brief Returns the current value
		virtual QVariant getCurrentValue(void) const = 0;

		//! @brief Creates a property configuration that can be used to later create this object
		virtual Property* createPropertyConfiguration(void) const = 0;

		//! @brief Returns the root widget of the input (allows nested widgets)
		virtual QWidget* getQWidget(void) override = 0;

		//! @brief Setup the input from the provided configration
		virtual bool setupFromConfiguration(const Property* _configuration);

		void setPropertyFlags(Property::PropertyFlags _flags) { m_flags = m_flags; };
		Property::PropertyFlags propertyFlags(void) const { return m_flags; };

		void setDataHasChanged(bool _changed = true) { m_dataChanged = _changed; };
		bool dataHasChanged(void) const { return m_dataChanged; };

		void setPropertyName(const std::string& _name) { m_propertyName = _name; };
		const std::string propertyName(void) const { return m_propertyName; };

		void setPropertyTitle(const QString& _title) { m_propertyTitle =_title; };
		const QString& propertyTitle(void) const { return m_propertyTitle; };

		void setPropertyTip(const QString& _tip) { m_propertyTip = _tip; };
		const QString& propertyTip(void) const { return m_propertyTip; };

	Q_SIGNALS:
		//! @brief Is emitted whenever the user changed a value
		void inputValueChanged(void);

	public Q_SLOTS:
		//! @brief If called the "inputValueChanged()" signal is emitted and the multiple values flag removed
		void slotValueChanged(void);

		//! @brief If called the "inputValueChanged()" signal is emitted and the multiple values flag removed
		void slotValueChanged(bool);

		//! @brief If called the "inputValueChanged()" signal is emitted and the multiple values flag removed
		void slotValueChanged(int);

	private:
		std::string m_propertyName;
		QString m_propertyTitle;
		QString m_propertyTip;
		Property::PropertyFlags m_flags;
		bool m_dataChanged;
	};
}