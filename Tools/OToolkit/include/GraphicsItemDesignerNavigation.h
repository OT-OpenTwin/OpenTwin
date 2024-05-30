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

	void addRootItem(GraphicsItemDesignerItemBase* _item);

	bool isItemNameUnique(const QString& _itemName) const;

	bool updateItemName(const QString& _oldName, const QString& _newName);

	GraphicsItemDesignerItemBase* findDesignerItem(const QString& _itemName) const;

	ot::GraphicsItemCfg* generateConfig(void);

	GraphicsItemDesignerNavigationRoot* getDesignerRootItem(void) const { return m_rootItem; };

private Q_SLOTS:
	void slotSelectionChanged(void);

protected:
	virtual void keyPressEvent(QKeyEvent* _event) override;

	void forgetItem(GraphicsItemDesignerItemBase* _item);

private:
	QRectF calculateDesignerItemRect(void) const;
	
	std::list<GraphicsItemDesignerItemBase*> m_rootItems;
	std::map<QString, GraphicsItemDesignerItemBase*> m_itemsMap;
	
	GraphicsItemDesigner* m_designer;
	GraphicsItemDesignerNavigationRoot* m_rootItem;
	GraphicsItemDesignerPropertyHandler* m_currentPropertyHandler;
};