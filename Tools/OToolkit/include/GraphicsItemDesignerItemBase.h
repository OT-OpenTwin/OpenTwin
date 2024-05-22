//! @file GraphicsItemDesignerItemBase.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerPropertyHandler.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/IconManager.h"

// Qt header
#include <QtCore/qpoint.h>

// std header
#include <list>

namespace ot {
	class GraphicsItem;
}

class GraphicsItemDesignerItemBase : public GraphicsItemDesignerPropertyHandler {
	OT_DECL_NOCOPY(GraphicsItemDesignerItemBase)
public:
	GraphicsItemDesignerItemBase(); 
	virtual ~GraphicsItemDesignerItemBase();

	void addControlPoint(const QPointF& _pt);
	void setControlPoints(const std::list<QPointF>& _points);
	const std::list<QPointF>& controlPoints(void) const { return m_controlPoints; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Pure virtual methods

	//! \brief Rebuilds the item.
	//! The item will be rebuild according to currently set control points.
	//! 
	//! If the function returns true if the item is valid (e.g. A WrappedShapeItem has more than one point set and the starting point is equal to the end point).
	//! \return True if the item is valid.
	virtual bool rebuildItem(void) = 0;

	virtual ot::GraphicsItem* getGraphicsItem(void) = 0;

	virtual QString getDefaultItemName(void) const = 0;

	virtual ot::TreeWidgetItemInfo createNavigationInformation(void) const = 0;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected base class methods

protected:
	virtual void fillPropertyGrid(void) override = 0;
	virtual void propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override = 0;
	virtual void propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override = 0;

private:
	std::list<QPointF> m_controlPoints;

};