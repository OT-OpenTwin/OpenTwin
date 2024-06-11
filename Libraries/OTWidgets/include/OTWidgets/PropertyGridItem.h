//! @file PropertyGridEntry.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyBase.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/TreeWidgetItem.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <string>

class QWidget;

namespace ot {

	class Label;
	class Property;
	class PropertyInput;

	class OT_WIDGETS_API_EXPORT PropertyGridItem : public QObject, public TreeWidgetItem {
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

		void setGroupName(const std::string& _name) { m_groupName = _name; };
		const std::string& getGroupName(void) const { return m_groupName; };

		Property* createProperty(void) const;

		void setTitle(const QString& _title);
		QString getTitle(void) const;

		void setInput(PropertyInput* _input);
		PropertyInput* getInput(void) const { return m_input; };
		
		void setPropertyColor(const QColor& _color) { m_propertyColor = _color; };
		const QColor& propertyColor(void) const { return m_propertyColor; };

		PropertyBase getPropertyData(void) const;
		std::string getPropertyType(void) const;

	Q_SIGNALS:
		void inputValueChanged(void);
		void deleteRequested(void);

	private Q_SLOTS:
		void slotValueChanged(void);
		void slotDeleteRequested(void);
		void slotGlobalStyleChanged(const ColorStyle& _style);
		
	private:
		QWidget* m_titleLayoutW;
		Label* m_deleteLabel;
		Label* m_titleLabel;

		std::string m_groupName;
		PropertyInput* m_input;
		QColor m_propertyColor;
	};

}

Q_DECLARE_METATYPE(ot::PropertyGridItem*)