// @otlicense
// File: PropertyGridGroup.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/TreeWidgetItem.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtGui/qbrush.h>

class QWidget;

namespace ot {

	class Label;
	class PropertyGridItem;

	class OT_WIDGETS_API_EXPORT PropertyGridGroup : public QObject, public TreeWidgetItem {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyGridGroup)
		OT_DECL_NOMOVE(PropertyGridGroup)
		OT_DECL_NODEFAULT(PropertyGridGroup)
	public:
		PropertyGridGroup(QWidget* _parent);
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
		Label* m_titleIconLabel;
		Label* m_titleLabel;
	};

}