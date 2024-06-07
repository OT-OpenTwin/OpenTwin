//! @file WrappedTriangleItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedTriangleItem.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTGui/StyleRefPainter2D.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"

WrappedTriangleItem::WrappedTriangleItem() {
	this->setOutline(ot::OutlineF(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBackground));
}

WrappedTriangleItem::~WrappedTriangleItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Public base class methods

ot::TreeWidgetItemInfo WrappedTriangleItem::createNavigationInformation(void) {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Triangle.png"));

	return info;
}

void WrappedTriangleItem::makeItemTransparent(void) {
	this->setOutline(ot::OutlineF(0., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected base class methods

void WrappedTriangleItem::controlPointsChanged(void) {
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
	this->setTriangleSize(newSize);
}

void WrappedTriangleItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	PropertyGroup* generalGroup = new PropertyGroup("General");
	generalGroup->addProperty(new PropertyString("Name", this->getGraphicsItem()->getGraphicsItemName()));
	generalGroup->addProperty(new PropertyBool("Connectable", this->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsConnectable));

	PropertyGroup* geometryGroup = new PropertyGroup("Geometry");
	geometryGroup->addProperty(new PropertyDouble("X", this->pos().x()));
	geometryGroup->addProperty(new PropertyDouble("Y", this->pos().y()));
	geometryGroup->addProperty(new PropertyDouble("Width", this->getTriangleSize().width()));
	geometryGroup->addProperty(new PropertyDouble("Height", this->getTriangleSize().height()));
	{
		std::list<std::string> possibleSelection;
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleShapeToString(GraphicsTriangleItemCfg::Triangle));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleShapeToString(GraphicsTriangleItemCfg::Kite));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleShapeToString(GraphicsTriangleItemCfg::IceCone));

		geometryGroup->addProperty(new PropertyStringList("Shape", GraphicsTriangleItemCfg::triangleShapeToString(this->getTriangleShape()), possibleSelection));
	}
	{
		std::list<std::string> possibleSelection;
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleDirectionToString(GraphicsTriangleItemCfg::Up));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleDirectionToString(GraphicsTriangleItemCfg::Left));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleDirectionToString(GraphicsTriangleItemCfg::Right));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleDirectionToString(GraphicsTriangleItemCfg::Down));

		geometryGroup->addProperty(new PropertyStringList("Direction", GraphicsTriangleItemCfg::triangleDirectionToString(this->getTrianlgeDirection()), possibleSelection));
	}

	geometryGroup->addProperty(new PropertyPainter2D("Border Painter", this->getOutline().painter()));
	geometryGroup->addProperty(new PropertyDouble("Border Width", this->getOutline().width()));
	geometryGroup->addProperty(new PropertyPainter2D("Background Painter", this->getBackgroundPainter()));

	cfg.addRootGroup(generalGroup);
	cfg.addRootGroup(geometryGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedTriangleItem::propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {
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
		this->setGeometry(QRectF(this->pos(), ot::QtFactory::toQSize(this->getTriangleSize())));
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Y") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->prepareGeometryChange();
		this->setPos(this->x(), input->getValue());
		this->setGeometry(QRectF(this->pos(), ot::QtFactory::toQSize(this->getTriangleSize())));
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Width") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setTriangleSize(QSizeF(input->getValue(), this->getTriangleSize().height()));
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Height") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setTriangleSize(QSizeF(this->getTriangleSize().width(), input->getValue()));
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Shape") {
		PropertyInputStringList* input = dynamic_cast<PropertyInputStringList*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setTriangleShape(GraphicsTriangleItemCfg::stringToTriangleShape(input->getCurrentText().toStdString()));
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Direction") {
		PropertyInputStringList* input = dynamic_cast<PropertyInputStringList*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setTriangleDirection(GraphicsTriangleItemCfg::stringToTriangleDirection(input->getCurrentText().toStdString()));
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
}

void WrappedTriangleItem::propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {

}

QVariant WrappedTriangleItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsTriangleItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}