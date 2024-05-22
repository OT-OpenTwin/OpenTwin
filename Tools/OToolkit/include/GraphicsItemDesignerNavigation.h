//! @file GraphicsItemDesignerNavigation.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/TreeWidget.h"

// std header
#include <list>

class GraphicsItemDesignerItemBase;

class GraphicsItemDesignerNavigation : public ot::TreeWidget {
public:
	GraphicsItemDesignerNavigation();
	virtual ~GraphicsItemDesignerNavigation();

	void addRootItem(GraphicsItemDesignerItemBase* _item);

private:
	void rebuildNavigation(void);

	std::list<GraphicsItemDesignerItemBase*> m_rootItems;

};