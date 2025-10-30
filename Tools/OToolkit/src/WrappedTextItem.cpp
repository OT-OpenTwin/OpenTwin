// @otlicense

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

static WrappedItemFactoryRegistrar<WrappedTextItem> circleRegistrar(ot::GraphicsTextItemCfg::className());

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

	this->setGraphicsItemPos(p1);
}

void WrappedTextItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	this->fillBasePropertyGrid(cfg);

	PropertyGroup* textGroup = new PropertyGroup("Text");
	textGroup->addProperty(new PropertyString("Text", this->getText()));
	{
		PropertyBool* isReferenceProp = new PropertyBool("Text As Reference", this->getTextIsReference());
		isReferenceProp->setPropertyTip("If enabled the text will be evaluated from the string map when then item is created by the GraphicsItemFactory.");
		textGroup->addProperty(isReferenceProp);
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
			textGroup->addProperty(new PropertyStringList("Font Family", newFontFamily, fontFamilies));
		}
	}

	textGroup->addProperty(new PropertyInt("Text Size", this->getFont().size()));
	textGroup->addProperty(new PropertyBool("Bold", this->getFont().isBold()));
	textGroup->addProperty(new PropertyBool("Italic", this->getFont().isItalic()));
	textGroup->addProperty(new PropertyPainter2D("Text Painter", this->getTextPainter()));

	cfg.addRootGroup(textGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedTextItem::propertyChanged(const ot::Property* _property) {
	using namespace ot;

	if (this->basePropertyChanged(_property)) return;

	const ot::PropertyGroup* group = _property->getParentGroup();
	if (!group) {
		OT_LOG_EA("Data mismatch");
		return;
	}

	if (group->getName() == "Text" && _property->getPropertyName() == "Text") {
		const PropertyString* actualProperty = dynamic_cast<const PropertyString*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setText(actualProperty->getValue());
	}
	else if (group->getName() == "Text" && _property->getPropertyName() == "Text As Reference") {
		const PropertyBool* actualProperty = dynamic_cast<const PropertyBool*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setTextIsReference(actualProperty->getValue());
	}
	else if (group->getName() == "Text" && _property->getPropertyName() == "Font Family") {
		const PropertyStringList* actualProperty = dynamic_cast<const PropertyStringList*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		Font font = this->getFont();
		font.setFamily(actualProperty->getCurrent());
		this->setFont(font);
	}
	else if (group->getName() == "Text" && _property->getPropertyName() == "Text Size") {
		const PropertyInt* actualProperty = dynamic_cast<const PropertyInt*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		Font font = this->getFont();
		font.setSize(actualProperty->getValue());
		this->setFont(font);
	}
	else if (group->getName() == "Text" && _property->getPropertyName() == "Bold") {
		const PropertyBool* actualProperty = dynamic_cast<const PropertyBool*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		Font font = this->getFont();
		font.setBold(actualProperty->getValue());
		this->setFont(font);
	}
	else if (group->getName() == "Text" && _property->getPropertyName() == "Italic") {
		const PropertyBool* actualProperty = dynamic_cast<const PropertyBool*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		Font font = this->getFont();
		font.setItalic(actualProperty->getValue());
		this->setFont(font);
	}
	else if (group->getName() == "Text" && _property->getPropertyName() == "Text Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setTextPainter(actualProperty->getPainter()->createCopy());
	}
	else {
		OT_LOG_E("Unknown property { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
	}
}

void WrappedTextItem::propertyDeleteRequested(const ot::Property* _property) {
	if (this->basePropertyDeleteRequested(_property)) return;

}

QVariant WrappedTextItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsTextItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}

