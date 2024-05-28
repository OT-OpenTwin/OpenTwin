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
#include <QtCore/qlist.h>
#include <QtCore/qpoint.h>

namespace ot {
	class GraphicsItem;
}

class GraphicsItemDesignerItemBase : public GraphicsItemDesignerPropertyHandler {
	OT_DECL_NOCOPY(GraphicsItemDesignerItemBase)
public:
	GraphicsItemDesignerItemBase(); 
	virtual ~GraphicsItemDesignerItemBase();

	void addControlPoint(const QPointF& _pt);
	void setControlPoints(const QList<QPointF>& _points);
	const QList<QPointF>& getControlPoints(void) const { return m_controlPoints; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Pure virtual methods

	virtual bool isDesignedItemCompleted(void) const = 0;

	virtual ot::GraphicsItem* getGraphicsItem(void) = 0;

	virtual QString getDefaultItemName(void) const = 0;

	virtual ot::TreeWidgetItemInfo createNavigationInformation(void) = 0;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected base class methods

protected:
	virtual void controlPointsChanged(void) {};
	virtual void fillPropertyGrid(void) override = 0;
	virtual void propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override = 0;
	virtual void propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override = 0;

private:
	QList<QPointF> m_controlPoints;

};