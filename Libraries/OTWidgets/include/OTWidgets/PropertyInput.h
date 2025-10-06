//! @file PropertyInput.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTGui/PropertyBase.h"
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

namespace ot {

	class Property;
	class PropertyGridItem;

	class OT_WIDGETS_API_EXPORT PropertyInput : public QObject, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInput)
	public:
		PropertyInput();
		virtual ~PropertyInput();

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
		virtual const QWidget* getQWidget(void) const override = 0;

		//! @brief Setup the input from the provided configration
		virtual bool setupFromConfiguration(const Property* _configuration);

		//! \brief Sets the focus on the input widget.
		virtual void focusPropertyInput(void) = 0;

		const std::string& getPropertyType(void) const { return m_type; };

		void setData(const PropertyBase& _data) { m_data = _data; };
		PropertyBase& data(void) { return m_data; };
		const PropertyBase& data(void) const { return m_data; };

		void setDataHasChanged(bool _changed = true) { m_dataChanged = _changed; };
		bool dataHasChanged(void) const { return m_dataChanged; };

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

	private Q_SLOTS:
		void slotEmitValueChanged(void);

	private:
		std::string m_type;
		PropertyBase m_data;
		bool m_dataChanged;
	};
}