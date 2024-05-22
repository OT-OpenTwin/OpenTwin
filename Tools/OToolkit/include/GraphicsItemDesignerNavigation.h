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

class GraphicsItemDesignerItemBase;

class QTreeWidgetItem;

class GraphicsItemDesignerNavigation : public ot::TreeWidget {
public:
	GraphicsItemDesignerNavigation();
	virtual ~GraphicsItemDesignerNavigation();

	void addRootItem(GraphicsItemDesignerItemBase* _item);

private:
	std::list<GraphicsItemDesignerItemBase*> m_rootItems;
	std::map<QString, GraphicsItemDesignerItemBase*> m_itemsMap;
	QTreeWidgetItem* m_rootItem;
};