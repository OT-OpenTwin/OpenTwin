//! @file WrappedEllipseItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedEllipseItem.h"
#include "WrappedItemFactory.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"

static WrappedItemFactoryRegistrar<WrappedEllipseItem> circleRegistrar(OT_FactoryKey_GraphicsEllipseItem);

WrappedEllipseItem::WrappedEllipseItem() {
	this->setOutline(ot::OutlineF(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBackground));
}

WrappedEllipseItem::~WrappedEllipseItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Public base class methods

ot::TreeWidgetItemInfo WrappedEllipseItem::createNavigationInformation(void) {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Ellipse.png"));

	return info;
}

void WrappedEllipseItem::makeItemTransparent(void) {
	this->setOutline(ot::OutlineF(0., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent));
}

void WrappedEllipseItem::setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) {
	if (!this->setupFromConfig(_config)) return;

	QRectF newRect(this->pos(), QSizeF(this->getRadiusX() * 2., this->getRadiusY() * 2.));

	QList<QPointF> newControlPoints;
	newControlPoints.append(newRect.topLeft());
	newControlPoints.append(newRect.bottomRight());

	this->initializeBaseData(newControlPoints, newRect.topLeft());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected base class methods

void WrappedEllipseItem::controlPointsChanged(void) {
	if (this->getControlPoints().size() != 2) return;

	ot::GraphicsScene* gscene = this->getGraphicsScene();
	if (!gscene) {
		OT_LOG_E("Graphics scene not set");
		return;
	}

	this->prepareGeometryChange();

	QPointF p1 = this->getControlPoints().front();
	QPointF p2 = this->getControlPoints().back();

	QPointF topLeft = QPointF(std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y()));
	QSizeF newSize = QSizeF(
		std::max(p1.x(), p2.x()) - std::min(p1.x(), p2.x()),
		std::max(p1.y(), p2.y()) - std::min(p1.y(), p2.y())
	);

	this->setPos(topLeft);
	this->setRadius(newSize.width() / 2., newSize.height() / 2.);
}

void WrappedEllipseItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	PropertyGroup* generalGroup = new PropertyGroup("General");
	generalGroup->addProperty(new PropertyString("Name", this->getGraphicsItem()->getGraphicsItemName()));
	generalGroup->addProperty(new PropertyBool("Connectable", this->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsConnectable));

	PropertyGroup* geometryGroup = new PropertyGroup("Geometry");
	geometryGroup->addProperty(new PropertyDouble("X", this->pos().x()));
	geometryGroup->addProperty(new PropertyDouble("Y", this->pos().y()));
	geometryGroup->addProperty(new PropertyDouble("Radius X", this->getRadiusX()));
	geometryGroup->addProperty(new PropertyDouble("Radius Y", this->getRadiusY()));
	geometryGroup->addProperty(new PropertyPainter2D("Border Painter", this->getOutline().painter()));
	geometryGroup->addProperty(new PropertyDouble("Border Width", this->getOutline().width()));
	geometryGroup->addProperty(new PropertyPainter2D("Background Painter", this->getBackgroundPainter()));
	{
		PropertyBool* newStateProperty = new PropertyBool("Handle State", this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState);
		newStateProperty->setPropertyTip("If enabled the item will update its appearance according to the current item state (e.g. ItemSelected or ItemHover)");
		geometryGroup->addProperty(newStateProperty);
	}


	cfg.addRootGroup(generalGroup);
	cfg.addRootGroup(geometryGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedEllipseItem::propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {
	using namespace ot;

	if (_item->getGroupName() == "General" && _itemData.propertyName() == "Name") {
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
	if (_item->getGroupName() == "General" && _itemData.propertyName() == "Connectable") {
		PropertyInputBool* input = dynamic_cast<PropertyInputBool*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed { \"Group\": \"" + _item->getGroupName() + "\", \"");
			return;
		}

		this->setGraphicsItemFlag(GraphicsItemCfg::ItemIsConnectable, input->isChecked());
		this->update();
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "X") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->prepareGeometryChange();
		this->setPos(input->getValue(), this->y());
		this->setGeometry(QRectF(this->pos(), QSizeF(this->getRadiusX() * 2., this->getRadiusY() * 2.)));
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Y") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->prepareGeometryChange();
		this->setPos(this->x(), input->getValue());
		this->setGeometry(QRectF(this->pos(), QSizeF(this->getRadiusX() * 2., this->getRadiusY() * 2.)));
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Radius X") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setRadiusX(input->getValue());
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Radius Y") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setRadiusY(input->getValue());
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Border Painter") {
		PropertyInputPainter2D* input = dynamic_cast<PropertyInputPainter2D*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		ot::OutlineF lineStyle = this->getOutline();
		lineStyle.setPainter(input->getPainter()->createCopy());
		this->setOutline(lineStyle);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Border Width") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		ot::OutlineF lineStyle = this->getOutline();
		lineStyle.setWidth(input->getValue());
		this->setOutline(lineStyle);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Background Painter") {
		PropertyInputPainter2D* input = dynamic_cast<PropertyInputPainter2D*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setBackgroundPainter(input->getPainter()->createCopy());
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Handle State") {
		PropertyInputBool* input = dynamic_cast<PropertyInputBool*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemHandlesState, input->isChecked());
	}
}

void WrappedEllipseItem::propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {

}

QVariant WrappedEllipseItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsEllipseItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}