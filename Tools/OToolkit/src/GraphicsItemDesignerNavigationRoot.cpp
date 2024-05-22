//! @file GraphicsItemDesignerNavigationRoot.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerNavigation.h"
#include "GraphicsItemDesignerNavigationRoot.h"
#include "GraphicsItemDesignerView.h"

GraphicsItemDesignerNavigationRoot::GraphicsItemDesignerNavigationRoot() 
	: m_view(nullptr)
{

}

void GraphicsItemDesignerNavigationRoot::fillPropertyGrid(void) {
	using namespace ot;
	
	PropertyGridCfg cfg;
	PropertyGroup* generalGroup = new PropertyGroup("General");
	generalGroup->addProperty(new PropertyString("Name", this->text(0).toStdString()));
	generalGroup->addProperty(new PropertyDouble("Width", m_view->sceneRect().width()));
	generalGroup->addProperty(new PropertyDouble("Height", m_view->sceneRect().height()));

	cfg.addRootGroup(generalGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}
	
void GraphicsItemDesignerNavigationRoot::propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {
	using namespace ot;
	
	if (_item->getGroupName() == "General" && _itemData.propertyName() == "Name") {
		PropertyInputString* input = dynamic_cast<PropertyInputString*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setText(0, input->getCurrentText());
	}
	else if (_item->getGroupName() == "General" && _itemData.propertyName() == "Width") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		m_view->setItemSize(QSizeF(input->getValue(), m_view->getItemSize().height()));
	}
	else if (_item->getGroupName() == "General" && _itemData.propertyName() == "Height") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		m_view->setItemSize(QSizeF(m_view->getItemSize().width(), input->getValue()));
	}
	else {
		OT_LOG_E("Unknown property { \"Group\": \"" + _item->getGroupName() + "\", \"Item\": \"" + _itemData.propertyName() + "\" }");
	}
}

void GraphicsItemDesignerNavigationRoot::propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {

}