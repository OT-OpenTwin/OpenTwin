//! @file PropertyGridGroup.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/TreeWidgetItem.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtGui/qbrush.h>

class QLabel;
class QWidget;

namespace ot {

	class PropertyGridItem;

	class OT_WIDGETS_API_EXPORT PropertyGridGroup : public QObject, public TreeWidgetItem {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyGridGroup)
	public:
		PropertyGridGroup();
		virtual ~PropertyGridGroup();

		void setupFromConfig(const PropertyGroup* _group);

		PropertyGroup* createConfiguration(void) const;

		//! @brief Finish the setup (should be called after the item is placed in a tree and after calling setup from config)
		void finishSetup(void);

		void setParentPropertyGroup(PropertyGridGroup* _group) { m_parentGroup = _group; };
		PropertyGridGroup* getParentPropertyGroup(void) const { return m_parentGroup; };

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setTitle(const QString& _title);
		QString getTitle(void) const;

		void addProperty(PropertyGridItem* _item);

		void addChildGroup(PropertyGridGroup* _group);

		const QColor& groupColor(void) const { return m_groupColor; };
		const QColor& alternateGroupColor(void) const { return m_groupAlternateColor; };

		PropertyGridItem* findChildProperty(const std::string& _propertyName, bool _searchChildGroups) const;
		std::list<PropertyGridItem*> childProperties(void) const;

		PropertyGridGroup* findChildGroup(const std::string& _name, bool _searchChildGroups) const;
		std::list<PropertyGridGroup*> childGroups(void) const;

		void updateStateIcon(void);

	Q_SIGNALS:
		void itemInputValueChanged(const std::string& _groupName, const std::string& _itemName);
		void itemInputValueChanged(const std::list<std::string>& _groupPath, const std::string& _itemName);
		void itemDeleteRequested(const std::string& _groupName, const std::string& _itemName);
		void itemDeleteRequested(const std::list<std::string>& _groupPath, const std::string& _itemName);

	private Q_SLOTS:
		void slotColorStyleChanged(const ColorStyle& _style);
		void slotItemInputValueChanged(void);
		void slotItemInputValueChanged(const std::string& _groupName, const std::string& _itemName);
		void slotItemInputValueChanged(const std::list<std::string>& _groupPath, const std::string& _itemName);
		void slotItemDeleteRequested(void);
		void slotItemDeleteRequested(const std::string& _groupName, const std::string& _itemName);
		void slotItemDeleteRequested(const std::list<std::string>& _groupPath, const std::string& _itemName);

	private:
		void updateStateIcon(const ColorStyle& _style);

		PropertyGridGroup* m_parentGroup;

		bool m_isAlternate;
		std::string m_name;
		QWidget* m_titleLayoutW;
		QLabel* m_titleIconLabel;
		QLabel* m_titleLabel;
		QColor m_groupColor;
		QColor m_groupAlternateColor;
	};

}