//! @file GraphicsItemDesignerItemBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerItemBase.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

GraphicsItemDesignerItemBase::GraphicsItemDesignerItemBase() 
	: m_designerItemFlags(DesignerItemFlag::NoDesignerItemFlags)
{

}

GraphicsItemDesignerItemBase::~GraphicsItemDesignerItemBase() {

}

void GraphicsItemDesignerItemBase::addControlPoint(const QPointF& _pt) {
	m_controlPoints.push_back(_pt);
	this->controlPointsChanged();
}

void GraphicsItemDesignerItemBase::setControlPoints(const QList<QPointF>& _points) {
	m_controlPoints = _points;
	this->controlPointsChanged();
}

void GraphicsItemDesignerItemBase::graphicsItemWasMoved(const QPointF& _newPos) {
	if (m_designerItemFlags & DesignerItemFlag::DesignerItemIgnoreEvents) return;

	// Calculate move delta
	QPointF delta = _newPos - m_lastPos;
	
	// Check if the item was moved
	if (delta.x() == 0 && delta.y() == 0) return;

	// Move all control points
	for (QPointF& pt : m_controlPoints) {
		pt += delta;
	}

	// Set the last pos
	m_lastPos = _newPos;
}

void GraphicsItemDesignerItemBase::initializeBaseData(const QList<QPointF>& _controlPoints, const QPointF& _pos) {
	m_controlPoints = _controlPoints;
	m_lastPos = _pos;
}

void GraphicsItemDesignerItemBase::fillBasePropertyGrid(ot::PropertyGridCfg& _config) {
	using namespace ot;
	PropertyGroup* generalGroup = new PropertyGroup("General");
	PropertyString* nameProp = new PropertyString("Name", this->getGraphicsItem()->getGraphicsItemName());
	nameProp->setPropertyTip("Item name. The name can be used to find it in a group of items.");
	generalGroup->addProperty(nameProp);
	PropertyDouble* posXProp = new PropertyDouble("Position X", this->getGraphicsItem()->getGraphicsItemPos().x(), 0., DBL_MAX);
	posXProp->setPropertyTip("Item top left X position.");
	generalGroup->addProperty(posXProp);
	PropertyDouble* posYProp = new PropertyDouble("Position Y", this->getGraphicsItem()->getGraphicsItemPos().y(), 0., DBL_MAX);
	posYProp->setPropertyTip("Item top left Y position.");
	generalGroup->addProperty(posYProp);

	PropertyBool* newConnectableProperty = new PropertyBool("Connectable", this->getGraphicsItem()->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsConnectable);
	newConnectableProperty->setPropertyTip("If enabled the item can be used as origin/destination for connections. The name of the item must be set.");
	generalGroup->addProperty(newConnectableProperty);
	
	PropertyBool* newStateProperty = new PropertyBool("Handle State", this->getGraphicsItem()->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState);
	newStateProperty->setPropertyTip("If enabled the item will update its appearance according to the current item state (e.g. ItemSelected or ItemHover)");
	generalGroup->addProperty(newStateProperty);

	PropertyGroup* transformGroup = new PropertyGroup("Transform");
	transformGroup->addProperty(new PropertyDouble("Rotation", this->getGraphicsItem()->getGraphicsItemTransform().getRotation(), 0., 359.99));
	transformGroup->addProperty(new PropertyBool("Flip Horizontal", this->getGraphicsItem()->getGraphicsItemTransform().getFlipStateFlags() & Transform::FlipHorizontally));
	transformGroup->addProperty(new PropertyBool("Flip Vertical", this->getGraphicsItem()->getGraphicsItemTransform().getFlipStateFlags() & Transform::FlipVertically));

	_config.addRootGroup(generalGroup);
	_config.addRootGroup(transformGroup);
}

bool GraphicsItemDesignerItemBase::basePropertyChanged(const ot::Property* _property) {
	using namespace ot;

	const PropertyGroup* group = _property->getParentGroup();
	if (!group) {
		return false;
	}

	// GENERAL
	if (group->getName() == "General" && _property->getPropertyName() == "Name") {
		const PropertyString* actualProperty = dynamic_cast<const PropertyString*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return false;
		}

		OTAssertNullptr(this->getNavigation());
		if (actualProperty->getValue().empty()) return true;
		if (!this->getNavigation()->updateItemName(QString::fromStdString(this->getGraphicsItem()->getGraphicsItemName()), QString::fromStdString(actualProperty->getValue()))) {
			return true;
		}

		this->getGraphicsItem()->setGraphicsItemName(actualProperty->getValue());
		return true;
	}
	else if (group->getName() == "General" && _property->getPropertyName() == "Position X") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return false;
		}

		Point2DD newPos = this->getGraphicsItem()->getGraphicsItemPos();
		newPos.setX(actualProperty->getValue());
		this->getGraphicsItem()->setGraphicsItemPos(newPos);
		return true;
	}
	else if (group->getName() == "General" && _property->getPropertyName() == "Position Y") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return false;
		}

		Point2DD newPos = this->getGraphicsItem()->getGraphicsItemPos();
		newPos.setY(actualProperty->getValue());
		this->getGraphicsItem()->setGraphicsItemPos(newPos);
		return true;
	}
	else if (group->getName() == "General" && _property->getPropertyName() == "Connectable") {
		const PropertyBool* actualProperty = dynamic_cast<const PropertyBool*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return false;
		}

		this->getGraphicsItem()->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemIsConnectable, actualProperty->getValue());
		return true;
	}
	else if (group->getName() == "General" && _property->getPropertyName() == "Handle State") {
		const PropertyBool* actualProperty = dynamic_cast<const PropertyBool*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return false;
		}

		this->getGraphicsItem()->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemHandlesState, actualProperty->getValue());
		return true;
	}

	// TRANSFORM ##########################################################################
	else if (group->getName() == "Transform" && _property->getPropertyName() == "Rotation") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return false;
		}

		Transform newTransform = this->getGraphicsItem()->getGraphicsItemTransform();
		newTransform.setRotation(actualProperty->getValue());
		this->getGraphicsItem()->setGraphicsItemTransform(newTransform);
		return true;
	}
	else if (group->getName() == "Transform" && _property->getPropertyName() == "Flip Horizontal") {
		const PropertyBool* actualProperty = dynamic_cast<const PropertyBool*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return false;
		}

		Transform newTransform = this->getGraphicsItem()->getGraphicsItemTransform();
		newTransform.setFlipState(Transform::FlipHorizontally, actualProperty->getValue());
		this->getGraphicsItem()->setGraphicsItemTransform(newTransform);
		return true;
	}
	else if (group->getName() == "Transform" && _property->getPropertyName() == "Flip Vertical") {
		const PropertyBool* actualProperty = dynamic_cast<const PropertyBool*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return false;
		}

		Transform newTransform = this->getGraphicsItem()->getGraphicsItemTransform();
		newTransform.setFlipState(Transform::FlipVertically, actualProperty->getValue());
		this->getGraphicsItem()->setGraphicsItemTransform(newTransform);
		return true;
	}

	return false;
}

bool GraphicsItemDesignerItemBase::basePropertyDeleteRequested(const ot::Property* _property) {
	return false;
}
