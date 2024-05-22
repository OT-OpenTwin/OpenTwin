//! @file GraphicsItemDesignerNavigationRoot.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerPropertyHandler.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

class GraphicsItemDesignerView;

class GraphicsItemDesignerNavigationRoot : public QTreeWidgetItem, public GraphicsItemDesignerPropertyHandler {
public:
	GraphicsItemDesignerNavigationRoot();
	virtual ~GraphicsItemDesignerNavigationRoot() {};

	void setDesignerView(GraphicsItemDesignerView* _view) { m_view = _view; };

protected:
	virtual void fillPropertyGrid(void) override;
	virtual void propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override;
	virtual void propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override;

private:
	GraphicsItemDesignerView* m_view;
};