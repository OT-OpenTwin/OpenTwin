//! @file PropertyGridEntry.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/Property.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtWidgets/qtreewidget.h>

namespace ot {

	class Property;
	class PropertyInput;

	class OT_WIDGETS_API_EXPORT PropertyGridItem : public QObject, public QTreeWidgetItem {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyGridItem)
	public:
		PropertyGridItem();
		virtual ~PropertyGridItem();

		//! @brief Setup the item
		//! Note that the item must be placed in a tree before calling the setup
		virtual bool setupFromConfig(const Property* _config);

		//! @brief Finish the setup (should be called after the item is placed in a tree and after calling setup from config)
		void finishSetup(void);

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setGroupName(const std::string& _name) { m_groupName = _name; };
		const std::string& getGroupName(void) const { return m_groupName; };

		void setPropertySpecialType(const std::string& _type) { m_specialType = _type; };
		const std::string& propertySpecialType(void) const { return m_specialType; };

		void setAdditionalPropertyData(const std::string& _data) { m_data = _data; };
		const std::string& additionalPropertyData(void) const { return m_data; };

		Property::PropertyType getPropertyType(void) const { return m_type; };

		Property* createProperty(void) const;

		void setTitle(const QString& _title);
		QString getTitle(void) const;

		void setInput(PropertyInput* _input);
		PropertyInput* getInput(void) const { return m_input; };
		
		void setPropertyBrush(const QBrush& _brush) { m_propertyBrush = _brush; };
		const QBrush& propertyBrush(void) const { return m_propertyBrush; };

		bool isPropertyDeletable(void) const { return m_isDeleteable; };

	Q_SIGNALS:
		void inputValueChanged(void);
		void deleteRequested(void);

	private Q_SLOTS:
		void slotValueChanged(void);

	private:
		bool m_isDeleteable;
		std::string m_name;
		std::string m_groupName;
		std::string m_specialType;
		std::string m_data;
		Property::PropertyType m_type;
		PropertyInput* m_input;
		QBrush m_propertyBrush;
	};

}

Q_DECLARE_METATYPE(ot::PropertyGridItem*)