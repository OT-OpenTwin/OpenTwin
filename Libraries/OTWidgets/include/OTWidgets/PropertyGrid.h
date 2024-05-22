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

	class TreeWidget;
	class PropertyGridItem;
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

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		TreeWidget* getTreeWidget(void) const;

		void setupGridFromConfig(const PropertyGridCfg& _config);
		void addRootItem(PropertyGridItem* _item);
		void addGroup(PropertyGridGroup* _group);

		PropertyGridGroup* findGroup(const std::string& _groupName) const;
		PropertyGridItem* findItem(const std::string& _itemName) const;
		PropertyGridItem* findItem(const std::string& _groupName, const std::string& _itemName) const;

		void clear(void);

	Q_SIGNALS:
		void propertyChanged(const std::string& _groupName, const std::string& _propertyName);
		void propertyDeleteRequested(const std::string& _groupName, const std::string& _propertyName);

	private Q_SLOTS:
		void slotPropertyChanged(void);
		void slotPropertyChanged(const std::string& _groupName, const std::string& _itemName);
		void slotPropertyDeleteRequested(void);
		void slotPropertyDeleteRequested(const std::string& _groupName, const std::string& _itemName);
		void slotItemCollapsed(QTreeWidgetItem* _item);
		void slotItemExpanded(QTreeWidgetItem* _item);

	private:
		class PropertyGridTree;
		PropertyGridTree* m_tree;
	};

}
