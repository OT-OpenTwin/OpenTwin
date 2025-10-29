// @otlicense

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
		OT_DECL_NOMOVE(PropertyGridGroup)
	public:
		PropertyGridGroup();
		virtual ~PropertyGridGroup();

		void setupFromConfig(const PropertyGroup* _group);

		PropertyGroup* createConfiguration(bool _includeChildAndProperties) const;

		//! @brief Finish the setup (should be called after the item is placed in a tree and after calling setup from config)
		void finishSetup();

		void setParentPropertyGroup(PropertyGridGroup* _group) { m_parentGroup = _group; };
		PropertyGridGroup* getParentPropertyGroup() const { return m_parentGroup; };

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName() const { return m_name; };

		void setTitle(const QString& _title);
		QString getTitle() const;

		void addProperty(PropertyGridItem* _item);

		void addChildGroup(PropertyGridGroup* _group);

		PropertyGridItem* findChildProperty(const std::string& _propertyName, bool _searchChildGroups) const;
		std::list<PropertyGridItem*> childProperties() const;

		PropertyGridGroup* findChildGroup(const std::string& _name, bool _searchChildGroups) const;
		std::list<PropertyGridGroup*> childGroups() const;

		void updateStateIcon();

	Q_SIGNALS:
		void itemInputValueChanged(const ot::Property* _property);
		void itemDeleteRequested(const ot::Property* _property);

	private Q_SLOTS:
		void slotColorStyleChanged();
		void slotItemInputValueChanged(const ot::Property* _property);
		void slotItemDeleteRequested(const ot::Property* _property);

	private:
		void updateStateIcon(const ColorStyle& _style);

		PropertyGridGroup* m_parentGroup;

		std::string m_name;
		QWidget* m_titleLayoutW;
		QLabel* m_titleIconLabel;
		QLabel* m_titleLabel;
	};

}