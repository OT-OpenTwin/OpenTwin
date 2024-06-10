//! @file WrappedTextItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedTextItem.h"
#include "WrappedItemFactory.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTGui/FillPainter2D.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsTextItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"

// Qt header
#include <QtGui/qfontdatabase.h>

static WrappedItemFactoryRegistrar<WrappedTextItem> circleRegistrar(OT_FactoryKey_GraphicsTextItem);

WrappedTextItem::WrappedTextItem() {
	this->setText("Text");
	this->setTextPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemForeground));
}

WrappedTextItem::~WrappedTextItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

ot::TreeWidgetItemInfo WrappedTextItem::createNavigationInformation(void) {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Text.png"));

	return info;
}

void WrappedTextItem::setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) {
	if (!this->setupFromConfig(_config)) return;

	QList<QPointF> newControlPoints;
	newControlPoints.append(this->pos());

	this->initializeBaseData(newControlPoints, this->pos());
}

// ###########################################################################################################################################################################################################################################################################################################################

void WrappedTextItem::controlPointsChanged(void) {
	if (this->getControlPoints().size() != 1) return;

	ot::GraphicsScene* gscene = this->getGraphicsScene();
	if (!gscene) {
		OT_LOG_E("Graphics scene not set");
		return;
	}

	this->prepareGeometryChange();

	QPointF p1 = this->getControlPoints().front();

	this->setPos(p1);
}

void WrappedTextItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	PropertyGroup* generalGroup = new PropertyGroup("General");
	generalGroup->addProperty(new PropertyString("Name", this->getGraphicsItem()->getGraphicsItemName()));

	PropertyGroup* geometryGroup = new PropertyGroup("Geometry");
	geometryGroup->addProperty(new PropertyDouble("X", this->pos().x()));
	geometryGroup->addProperty(new PropertyDouble("Y", this->pos().y()));
	geometryGroup->addProperty(new PropertyString("Text", this->getText()));
	{
		PropertyBool* isReferenceProp = new PropertyBool("Text As Reference", this->getTextIsReference());
		isReferenceProp->setPropertyTip("If enabled the text will be evaluated from the string map when then item is created by the GraphicsItemFactory.");
		geometryGroup->addProperty(isReferenceProp);
	}
	{
		std::list<std::string> fontFamilies;
		std::string newFontFamily = this->getFont().family();

		bool found = false;
		for (const QString& family : QFontDatabase::families()) {
			std::string newFamily = family.toStdString();
			if (newFamily == this->getFont().family()) found = true;
			fontFamilies.push_back(newFamily);
		}
		if (fontFamilies.empty()) {
			OT_LOG_E("No font families found");
		}
		else {
			if (!found) newFontFamily = fontFamilies.front();
			geometryGroup->addProperty(new PropertyStringList("Font Family", newFontFamily, fontFamilies));
		}
	}

	geometryGroup->addProperty(new PropertyInt("Text Size", this->getFont().size()));
	geometryGroup->addProperty(new PropertyBool("Bold", this->getFont().isBold()));
	geometryGroup->addProperty(new PropertyBool("Italic", this->getFont().isItalic()));
	geometryGroup->addProperty(new PropertyPainter2D("Text Painter", this->getTextPainter()));
	{
		PropertyBool* newStateProperty = new PropertyBool("Handle State", this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState);
		newStateProperty->setPropertyTip("If enabled the item will update its appearance according to the current item state (e.g. ItemSelected or ItemHover)");
		geometryGroup->addProperty(newStateProperty);
	}

	cfg.addRootGroup(generalGroup);
	cfg.addRootGroup(geometryGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedTextItem::propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {
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

		this->setPos(input->getValue(), this->pos().y());
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Y") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setPos(this->pos().x(), input->getValue());
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Text") {
		PropertyInputString* input = dynamic_cast<PropertyInputString*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setText(input->getCurrentText());
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Text As Reference") {
		PropertyInputBool* input = dynamic_cast<PropertyInputBool*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setTextIsReference(input->isChecked());
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Font Family") {
		PropertyInputStringList* input = dynamic_cast<PropertyInputStringList*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		Font font = this->getFont();
		font.setFamily(input->getCurrentText().toStdString());
		this->setFont(font);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Text Size") {
		PropertyInputInt* input = dynamic_cast<PropertyInputInt*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		Font font = this->getFont();
		font.setSize(input->getValue());
		this->setFont(font);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Bold") {
		PropertyInputBool* input = dynamic_cast<PropertyInputBool*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		Font font = this->getFont();
		font.setBold(input->isChecked());
		this->setFont(font);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Italic") {
		PropertyInputBool* input = dynamic_cast<PropertyInputBool*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		Font font = this->getFont();
		font.setItalic(input->isChecked());
		this->setFont(font);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.getPropertyName() == "Text Painter") {
		PropertyInputPainter2D* input = dynamic_cast<PropertyInputPainter2D*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setTextPainter(input->getPainter()->createCopy());
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

void WrappedTextItem::propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {

}

QVariant WrappedTextItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsTextItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}

