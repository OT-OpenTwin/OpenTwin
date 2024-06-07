//! @file GraphicsItemDesignerNavigation.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerExportConfig.h"

// OpenTwin header
#include "OTWidgets/TreeWidget.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <map>
#include <list>

class GraphicsItemDesigner;
class GraphicsItemDesignerView;
class GraphicsItemDesignerItemBase;
class GraphicsItemDesignerNavigationRoot;
class GraphicsItemDesignerPropertyHandler;

namespace ot { class PropertyGrid; }
namespace ot { class GraphicsItemCfg; }

class GraphicsItemDesignerNavigation : public ot::TreeWidget {
	Q_OBJECT
	OT_DECL_NOCOPY(GraphicsItemDesignerNavigation)
	OT_DECL_NODEFAULT(GraphicsItemDesignerNavigation)
public:
	GraphicsItemDesignerNavigation(GraphicsItemDesigner* _designer);
	virtual ~GraphicsItemDesignerNavigation();

	void addRootItem(GraphicsItemDesignerItemBase* _item, bool _keepName);

	bool isItemNameUnique(const QString& _itemName) const;

	bool updateItemName(const QString& _oldName, const QString& _newName);

	GraphicsItemDesignerItemBase* findDesignerItem(const QString& _itemName) const;

	ot::GraphicsItemCfg* generateConfig(const GraphicsItemDesignerExportConfig& _exportConfig);

	GraphicsItemDesignerNavigationRoot* getDesignerRootItem(void) const { return m_rootItem; };

	void setCurrentSelection(const std::list<std::string>& _itemNames);

	std::list<GraphicsItemDesignerItemBase*> getCurrentDesignerSelection(void) const;

	bool hasDesignerItems(void) const { return !m_itemsMap.empty(); };

	void updatePropertyGrid(void);

	//! \brief Removes and destroys all designer items.
	//! The items will be removed from the view aswell.
	void clearDesignerItems(void);

	void removeSelectedDesignerItems(void);

	void removeDesignerItems(const QStringList& _itemNames);

private Q_SLOTS:
	void slotSelectionChanged(void);
	
protected:
	virtual void keyPressEvent(QKeyEvent* _event) override;

	void forgetItem(GraphicsItemDesignerItemBase* _item);

private:
	QRectF calculateDesignerItemRect(void) const;
	
	std::list<GraphicsItemDesignerItemBase*> m_rootItems;
	std::map<QString, GraphicsItemDesignerItemBase*> m_itemsMap;
	
	bool m_selectionChangeInProgress;
	GraphicsItemDesigner* m_designer;
	GraphicsItemDesignerNavigationRoot* m_rootItem;
	GraphicsItemDesignerPropertyHandler* m_currentPropertyHandler;
};