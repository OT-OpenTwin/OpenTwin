//! @file GraphicsItemDesignerNavigationRoot.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerPropertyHandler.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

class GraphicsItemDesignerNavigationRoot : public QTreeWidgetItem, public GraphicsItemDesignerPropertyHandler {
public:
	GraphicsItemDesignerNavigationRoot();
	virtual ~GraphicsItemDesignerNavigationRoot() {};

protected:
	virtual void fillPropertyGrid(void) override;
	virtual void propertyChanged(const std::string& _group, const std::string& _item) override;
	virtual void propertyDeleteRequested(const std::string& _group, const std::string& _item) override;

private:

};