//! @file PropertyGrid.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PropertyGridCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/QWidgetInterface.h"

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

	class OT_WIDGETS_API_EXPORT PropertyGrid : public QObject, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyGrid)
	public:
		PropertyGrid(QObject* _parentObject = (QObject*)nullptr);
		virtual ~PropertyGrid();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getQWidget(void) override;
		virtual const QWidget* getQWidget(void) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		TreeWidget* getTreeWidget(void) const;

		void setupGridFromConfig(const PropertyGridCfg& _config);
		
		PropertyGridCfg createGridConfig(void) const;

		void addGroup(PropertyGridGroup* _group);

		PropertyGridGroup* findGroup(const std::string& _groupName) const;
		PropertyGridGroup* findGroup(const std::list<std::string>& _groupPath) const;
		PropertyGridItem* findItem(const std::string& _groupName, const std::string& _itemName) const;
		PropertyGridItem* findItem(const std::list<std::string>& _groupPath, const std::string& _itemName) const;
		std::list<PropertyGridItem*> getAllItems(void) const;

		void clear(void);

		void focusProperty(const std::string& _groupName, const std::string& _itemName);
		void focusProperty(const std::list<std::string>& _groupPath, const std::string& _itemName);

	Q_SIGNALS:
		void propertyChanged(const Property* const _property);
		void propertyDeleteRequested(const Property* const _property);

	private Q_SLOTS:
		void slotPropertyChanged(const Property* const _property);
		void slotPropertyDeleteRequested(const Property* const _property);
		void slotItemCollapsed(QTreeWidgetItem* _item);
		void slotItemExpanded(QTreeWidgetItem* _item);

	private:
		PropertyGridGroup* findGroup(QTreeWidgetItem* _parentTreeItem, const std::list<std::string>& _groupPath) const;
		void findAllChildItems(QTreeWidgetItem* _parentTreeItem, std::list<PropertyGridItem*>& _items) const;


		PropertyGridTree* m_tree;
	};

}
