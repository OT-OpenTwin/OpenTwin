// @otlicense

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
	class GraphicsItemCfg;
}

class GraphicsItemDesignerItemBase : public GraphicsItemDesignerPropertyHandler {
	OT_DECL_NOCOPY(GraphicsItemDesignerItemBase)
public:
	enum class DesignerItemFlag {
		NoDesignerItemFlags = 0x00,
		DesignerItemIgnoreEvents = 0x01
	};
	typedef ot::Flags<DesignerItemFlag> DesignerItemFlags;

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

	//! \brief Sets the last position.
	//! \see getLastPos
	void setLastPos(const QPointF& _pos) { m_lastPos = _pos; };

	//! \brief Returns the last position.
	//! The last position ist the last position of this graphics item.
	//! The position is used to adjust the control points.
	const QPointF& getLastPos(void) const { return m_lastPos; };

	void setDesignerItemFlag(DesignerItemFlag _flag, bool _active = true) { m_designerItemFlags.setFlag(_flag, _active); };
	void setDesignerItemFlags(const DesignerItemFlags& _flags) { m_designerItemFlags = _flags; };
	const DesignerItemFlags& getDesignerItemFlags(void) const { return m_designerItemFlags; };

	//! \brief Will move all control points by the move delta (lastPos - newPos).
	//! The method will set the last pos after moving.
	//! \param _newPos The position the item was moved to.
	void graphicsItemWasMoved(const QPointF& _newPos);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Virtual methods

	//! \brief Returns true if the current item is completed.
	//! A completed item has all required control points set (e.g. Line start and Line end) and no more points may be set.
	virtual bool isDesignedItemCompleted(void) const = 0;

	//! \brief Returns true if the current item is valid.
	//! A valid item has all required control points set (e.g. Line start and Line end).
	//! More points may be added (e.g. Polygon).
	virtual bool isDesignedItemValid(void) const = 0;

	//! \brief Returns the actual GraphicsItem.
	virtual ot::GraphicsItem* getGraphicsItem(void) = 0;

	//! \brief Returns the default item name.
	virtual QString getDefaultItemName(void) const = 0;

	//! \brief Creates the navigation information for this item and its childs.
	virtual ot::TreeWidgetItemInfo createNavigationInformation(void) = 0;

	//! \brief Will make the item transparent.
	virtual void makeItemTransparent(void) {};

	virtual void setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) = 0;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected base class methods

protected:
	//! \brief Is called whenever the control points have changed (except when calling updateControlPoints()).
	virtual void controlPointsChanged(void) = 0;

	//! \brief Is called when the property grid needs to be filled.
	virtual void fillPropertyGrid(void) override = 0;

	virtual void propertyChanged(const ot::Property* _property) override = 0;
	virtual void propertyDeleteRequested(const ot::Property* _property) override = 0;

	void initializeBaseData(const QList<QPointF>& _controlPoints, const QPointF& _pos);

	//! \brief Adds the default properties.
	//! Creates the "General" and "Transform" groups.
	void fillBasePropertyGrid(ot::PropertyGridCfg& _config);

	//! \brief Handles base properties.
	//! Returns true if the changed property was handled.
	bool basePropertyChanged(const ot::Property* _property);

	//! \brief Handles base properties.
	//! Returns true if the requested property was handled.
	bool basePropertyDeleteRequested(const ot::Property* _property);

private:
	QPointF m_lastPos; //! \see getLastPos
	QList<QPointF> m_controlPoints;
	DesignerItemFlags m_designerItemFlags;

};

OT_ADD_FLAG_FUNCTIONS(GraphicsItemDesignerItemBase::DesignerItemFlag)