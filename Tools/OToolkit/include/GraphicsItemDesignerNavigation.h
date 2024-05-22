//! @file GraphicsItemDesignerNavigation.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/TreeWidget.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <map>
#include <list>

class GraphicsItemDesignerView;
class GraphicsItemDesignerItemBase;
class GraphicsItemDesignerNavigationRoot;
class GraphicsItemDesignerPropertyHandler;

namespace ot { class PropertyGrid; }

class GraphicsItemDesignerNavigation : public ot::TreeWidget {
	Q_OBJECT
public:
	GraphicsItemDesignerNavigation();
	virtual ~GraphicsItemDesignerNavigation();

	void setDesignerView(GraphicsItemDesignerView* _view);

	void setPropertyGrid(ot::PropertyGrid* _grid) { m_propertyGrid = _grid; };

	void addRootItem(GraphicsItemDesignerItemBase* _item);

	bool isItemNameUnique(const QString& _itemName) const;

	bool updateItemName(const QString& _oldName, const QString& _newName);

private Q_SLOTS:
	void slotSelectionChanged(void);

private:
	std::list<GraphicsItemDesignerItemBase*> m_rootItems;
	std::map<QString, GraphicsItemDesignerItemBase*> m_itemsMap;
	
	GraphicsItemDesignerNavigationRoot* m_rootItem;
	
	ot::PropertyGrid* m_propertyGrid;
	GraphicsItemDesignerPropertyHandler* m_propertyHandler;
};