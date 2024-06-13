//! @file GraphicsItemDesignerNavigationRoot.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerPropertyHandler.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

class GraphicsItemDesigner;
class GraphicsItemDesignerView;

class GraphicsItemDesignerNavigationRoot : public QTreeWidgetItem, public GraphicsItemDesignerPropertyHandler {
public:
	GraphicsItemDesignerNavigationRoot(GraphicsItemDesigner* _designer);
	virtual ~GraphicsItemDesignerNavigationRoot() {};

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

protected:
	virtual void fillPropertyGrid(void) override;
	virtual void propertyChanged(const ot::Property* _property) override;
	virtual void propertyDeleteRequested(const ot::Property* _property) override;

private:
	GraphicsItemDesigner* m_designer;
};
