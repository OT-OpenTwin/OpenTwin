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

	//! \breif Adds the provided control point and calls controlPointsChanged().
	void addControlPoint(const QPointF& _pt);

	//! \brief Sets the current list of control points and calls controlPointsChanged().
	void setControlPoints(const QList<QPointF>& _points);
	
	//! \brief Replaces the current list of control points.
	//! This method does not call controlPointsChanged().
	void updateControlPoints(const QList<QPointF>& _points) { m_controlPoints = _points; };

	//! \brief Returns a list containing the currently set control points.
	const QList<QPointF>& getControlPoints(void) const { return m_controlPoints; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Virtual methods

	//! \brief Returns true if the current item is valid.
	//! A valid item has all required control points set (e.g. Line start and Line end).
	virtual bool isDesignedItemCompleted(void) const = 0;

	//! \brief Returns the actual GraphicsItem.
	virtual ot::GraphicsItem* getGraphicsItem(void) = 0;

	//! \brief Returns the default item name.
	virtual QString getDefaultItemName(void) const = 0;

	//! \brief Creates the navigation information for this item and its childs.
	virtual ot::TreeWidgetItemInfo createNavigationInformation(void) = 0;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected base class methods

protected:
	//! \brief Is called whenever the control points have changed (except when calling updateControlPoints()).
	virtual void controlPointsChanged(void) = 0;

	//! \brief Is called when the property grid needs to be filled.
	virtual void fillPropertyGrid(void) override = 0;

	virtual void propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override = 0;
	virtual void propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override = 0;

private:
	QList<QPointF> m_controlPoints;

};