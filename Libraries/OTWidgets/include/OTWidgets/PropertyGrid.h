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

		void setupFromConfig(const PropertyGridCfg& _config);
		void addRootItem(PropertyGridItem* _item);
		void addGroup(PropertyGridGroup* _group);

		const PropertyGridGroup* findGroup(const std::string& _groupName) const;
		const PropertyGridItem* findItem(const std::string& _itemName) const;
		const PropertyGridItem* findItem(const std::string& _groupName, const std::string& _itemName) const;

		void clear(void);

	private slots:
		void slotPropertyChanged();
		void slotItemChanged(QTreeWidgetItem* _item, int _column);

	private:
		class PropertyGridTree;
		PropertyGridTree* m_tree;
	};

}
