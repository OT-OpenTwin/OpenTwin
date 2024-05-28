//! @file GraphicsItemDesignerNavigationRoot.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerScene.h"
#include "GraphicsItemDesignerNavigation.h"
#include "GraphicsItemDesignerNavigationRoot.h"
#include "GraphicsItemDesignerView.h"

GraphicsItemDesignerNavigationRoot::GraphicsItemDesignerNavigationRoot() 
	: m_view(nullptr)
{
	this->setExpanded(true);
}

void GraphicsItemDesignerNavigationRoot::fillPropertyGrid(void) {
	using namespace ot;
	
	PropertyGridCfg cfg;
	PropertyGroup* editorGroup = new PropertyGroup("Editor");
	editorGroup->addProperty(new PropertyBool("Show grid", (m_view->getDesignerScene()->getGridFlags() | GraphicsItemDesignerScene::NoGridLineMask) == GraphicsItemDesignerScene::NoGridLineMask));
	PropertyInt* gridStepSizeProp = new PropertyInt("Grid step size", std::max(m_view->getDesignerScene()->getGridStepSize(), 2), 2, 1000);
	gridStepSizeProp->setPropertyTip("The distance between two grid lines.");
	editorGroup->addProperty(gridStepSizeProp);
	PropertyInt* wideStepProp = new PropertyInt("Wide step every", std::max(m_view->getDesignerScene()->getGridWideLineEvery(), 1), 1, 1000);
	wideStepProp->setPropertyTip("Every 'x' gridline will be a wide line (e.g. every 10th line).")	;
	editorGroup->addProperty(wideStepProp);
	PropertyBool* gridSnapProp = new PropertyBool("Grid snap", m_view->getDesignerScene()->getGridSnapEnabled());
	gridSnapProp->setPropertyTip("If enabled items will snap to the grid. This also works if the grid is hidden.");
	editorGroup->addProperty(gridSnapProp);

	PropertyGroup* generalGroup = new PropertyGroup("Item");
	generalGroup->addProperty(new PropertyString("Name", this->text(0).toStdString()));
	generalGroup->addProperty(new PropertyDouble("Width", m_view->sceneRect().width()));
	generalGroup->addProperty(new PropertyDouble("Height", m_view->sceneRect().height()));
	
	cfg.addRootGroup(editorGroup);
	cfg.addRootGroup(generalGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}
	
void GraphicsItemDesignerNavigationRoot::propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {
	using namespace ot;

	if (_item->getGroupName() == "Editor" && _itemData.propertyName() == "Show grid") {
		PropertyInputBool* input = dynamic_cast<PropertyInputBool*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}
		m_view->getDesignerScene()->setGridFlags((input->isChecked() ? (GraphicsScene::ShowNormalLines | GraphicsScene::ShowWideLines) : GraphicsScene::NoGridFlags));
		m_view->update();
	}
	else if (_item->getGroupName() == "Editor" && _itemData.propertyName() == "Grid step size") {
		PropertyInputInt* input = dynamic_cast<PropertyInputInt*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}
		m_view->getDesignerScene()->setGridStepSize(input->getValue());
		m_view->update();
	}
	else if (_item->getGroupName() == "Editor" && _itemData.propertyName() == "Wide step every") {
		PropertyInputInt* input = dynamic_cast<PropertyInputInt*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}
		m_view->getDesignerScene()->setGridWideLineEvery(input->getValue());
		m_view->update();
	}
	else if (_item->getGroupName() == "Editor" && _itemData.propertyName() == "Grid snap") {
		PropertyInputBool* input = dynamic_cast<PropertyInputBool*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}
		m_view->getDesignerScene()->setGridSnapEnabled(input->isChecked());
	}
	else if (_item->getGroupName() == "Item" && _itemData.propertyName() == "Name") {
		PropertyInputString* input = dynamic_cast<PropertyInputString*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setText(0, input->getCurrentText());
	}
	else if (_item->getGroupName() == "Item" && _itemData.propertyName() == "Width") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		m_view->setItemSize(QSizeF(input->getValue(), m_view->getItemSize().height()));
	}
	else if (_item->getGroupName() == "Item" && _itemData.propertyName() == "Height") {
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