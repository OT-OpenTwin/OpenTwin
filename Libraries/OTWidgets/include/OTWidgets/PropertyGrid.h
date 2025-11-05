// @otlicense
// File: PropertyGrid.h
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
#include "OTGui/PropertyGridCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qwidget.h>

// std header
#include <list>

class QTreeWidgetItem;

namespace ot {

	class Property;
	class TreeWidget;
	class PropertyGridItem;
	class PropertyGridTree;
	class PropertyGridGroup;

	class OT_WIDGETS_API_EXPORT PropertyGrid : public QObject, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyGrid)
		OT_DECL_NOMOVE(PropertyGrid)
		OT_DECL_NODEFAULT(PropertyGrid)
	public:
		explicit PropertyGrid(QWidget* _parent);
		virtual ~PropertyGrid();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getQWidget() override;
		virtual const QWidget* getQWidget() const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setIsModal(bool _isModal = true) { m_isModal = _isModal; };
		bool getIsModal() const { return m_isModal; };

		TreeWidget* getTreeWidget() const;

		void setupGridFromConfig(const PropertyGridCfg& _config);
		
		PropertyGridCfg createGridConfig() const;

		void addGroup(PropertyGridGroup* _group);

		PropertyGridGroup* findGroup(const std::string& _groupName) const;
		PropertyGridGroup* findGroup(const std::list<std::string>& _groupPath) const;
		PropertyGridItem* findItem(const std::string& _groupName, const std::string& _itemName) const;
		PropertyGridItem* findItem(const std::list<std::string>& _groupPath, const std::string& _itemName) const;
		std::list<PropertyGridItem*> getAllItems() const;

		void clear();

		void focusProperty(const std::string& _groupName, const std::string& _itemName);
		void focusProperty(const std::list<std::string>& _groupPath, const std::string& _itemName);

	Q_SIGNALS:
		void propertyChanged(const Property* _property);
		void propertyDeleteRequested(const Property* _property);

	private Q_SLOTS:
		void slotPropertyChanged(const Property* _property);
		void slotPropertyDeleteRequested(const Property* _property);
		void slotItemCollapsed(QTreeWidgetItem* _item);
		void slotItemExpanded(QTreeWidgetItem* _item);

	private:
		PropertyGridGroup* findGroup(QTreeWidgetItem* _parentTreeItem, const std::list<std::string>& _groupPath) const;
		void findAllChildItems(QTreeWidgetItem* _parentTreeItem, std::list<PropertyGridItem*>& _items) const;

		bool m_isModal;
		PropertyGridTree* m_tree;
	};

}
