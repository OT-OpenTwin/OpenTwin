//! @file WrappedArcItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedArcItem.h"
#include "WrappedItemFactory.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTGui/FillPainter2D.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsArcItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"

static WrappedItemFactoryRegistrar<WrappedArcItem> arcRegistrar(OT_FactoryKey_GraphicsArcItem);

WrappedArcItem::WrappedArcItem() {
	this->setLineStyle(ot::OutlineF(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
}

WrappedArcItem::~WrappedArcItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

ot::TreeWidgetItemInfo WrappedArcItem::createNavigationInformation(void) {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Arc.png"));

	return info;
}

void WrappedArcItem::setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) {
	if (!this->setupFromConfig(_config)) return;

	QRectF newRect = ot::QtFactory::toQRect(this->getArcRect());
	newRect.moveTo(this->pos());

	QList<QPointF> newControlPoints;
	newControlPoints.append(newRect.topLeft());
	newControlPoints.append(newRect.bottomRight());

	this->initializeBaseData(newControlPoints, newRect.topLeft());
}

// ###########################################################################################################################################################################################################################################################################################################################

void WrappedArcItem::controlPointsChanged(void) {
	if (this->getControlPoints().size() != 2) return;

	ot::GraphicsScene* gscene = this->getGraphicsScene();
	if (!gscene) {
		OT_LOG_E("Graphics scene not set");
		return;
	}

	this->prepareGeometryChange();

	QPointF topLeftPoint = QPointF(std::min(this->getControlPoints().front().x(), this->getControlPoints().back().x()), std::min(this->getControlPoints().front().y(), this->getControlPoints().back().y()));
	QPointF bottomRightPoint = QPointF(std::max(this->getControlPoints().front().x(), this->getControlPoints().back().x()), std::max(this->getControlPoints().front().y(), this->getControlPoints().back().y()));

	this->setPos(topLeftPoint);
	QRectF newArcRect(topLeftPoint, bottomRightPoint);
	newArcRect.moveTo(0., 0.);
	this->setArcRect(newArcRect);
}

void WrappedArcItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	PropertyGroup* generalGroup = new PropertyGroup("General");
	generalGroup->addProperty(new PropertyString("Name", this->getGraphicsItem()->getGraphicsItemName()));

	PropertyGroup* geometryGroup = new PropertyGroup("Geometry");
	geometryGroup->addProperty(new PropertyDouble("X", this->getArcRect().getTopLeft().x()));
	geometryGroup->addProperty(new PropertyDouble("Y", this->getArcRect().getTopLeft().y()));
	geometryGroup->addProperty(new PropertyDouble("Width", this->getArcRect().getWidth()));
	geometryGroup->addProperty(new PropertyDouble("Height", this->getArcRect().getHeight()));
	geometryGroup->addProperty(new PropertyDouble("Start Angle", this->getStartAngle() / 16.));
	geometryGroup->addProperty(new PropertyDouble("Span Angle", this->getSpanAngle() / 16.));
	geometryGroup->addProperty(new PropertyPainter2D("Line Painter", this->getLineStyle().painter()));
	geometryGroup->addProperty(new PropertyDouble("Line Width", this->getLineStyle().width()));
	{
		PropertyBool* newStateProperty = new PropertyBool("Handle State", this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState);
		newStateProperty->setPropertyTip("If enabled the item will update its appearance according to the current item state (e.g. ItemSelected or ItemHover)");
		geometryGroup->addProperty(newStateProperty);
	}

	cfg.addRootGroup(generalGroup);
	cfg.addRootGroup(geometryGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedArcItem::propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {
	using namespace ot;

	if (_item->getGroupName() == "General" && _itemData.getPropertyName() == "Name") {
		PropertyInputString* input = dynamic_cast<PropertyInputString*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed { \"Group\": \"" + _item->getGroupName() + "\", \"");
			return;
		}
		OTAssertNullptr(this->getNavigation());

		if (input->getCurrentText().isEmpty()) return;
		if (!this->getNavigation()->updateItemName(QString::fromStdString(this->getGraphicsItemName()), input->getCurrentText())) return;

		this->setGraphicsItemName(input->getCurrentText().toStdString());
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "X") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		RectD newRect = this->getArcRect();
		newRect.moveBy(input->getValue() - newRect.getLeft(), 0.);
		this->setArcRect(newRect);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Y") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		RectD newRect = this->getArcRect();
		newRect.moveBy(0., input->getValue() - newRect.getLeft());
		this->setArcRect(newRect);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Width") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		RectD newRect = this->getArcRect();
		this->setArcRect(RectD(newRect.getTopLeft(), Size2DD(input->getValue(), newRect.getHeight())));
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Height") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		RectD newRect = this->getArcRect();
		this->setArcRect(RectD(newRect.getTopLeft(), Size2DD(newRect.getHeight(), input->getValue())));
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Start Angle") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setStartAngle(input->getValue() * 16.);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Span Angle") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setSpanAngle(input->getValue() * 16.);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Line Painter") {
		PropertyInputPainter2D* input = dynamic_cast<PropertyInputPainter2D*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		ot::OutlineF lineStyle = this->getLineStyle();
		lineStyle.setPainter(input->getPainter()->createCopy());
		this->setLineStyle(lineStyle);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Line Width") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		ot::OutlineF lineStyle = this->getLineStyle();
		lineStyle.setWidth(input->getValue());
		this->setLineStyle(lineStyle);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Handle State") {
		PropertyInputBool* input = dynamic_cast<PropertyInputBool*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemHandlesState, input->isChecked());
	}
	
}

void WrappedArcItem::propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {

}

QVariant WrappedArcItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsArcItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}

