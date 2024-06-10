//! @file GraphicsItemDesignerNavigationRoot.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerScene.h"
#include "GraphicsItemDesignerNavigation.h"
#include "GraphicsItemDesignerNavigationRoot.h"

GraphicsItemDesignerNavigationRoot::GraphicsItemDesignerNavigationRoot(GraphicsItemDesigner* _designer)
	: m_designer(_designer)
{
	this->setExpanded(true);
	this->setText(0, "Container");
}

void GraphicsItemDesignerNavigationRoot::fillPropertyGrid(void) {
	using namespace ot;
	
	PropertyGridCfg cfg;
	PropertyGroup* editorGroup = new PropertyGroup("Editor");
	editorGroup->addProperty(new PropertyBool("Show grid", (m_designer->getView()->getDesignerScene()->getGridFlags() | Grid::NoGridLineMask) != Grid::NoGridLineMask));
	
	PropertyInt* gridStepSizeProp = new PropertyInt("Grid step size", std::max(m_designer->getView()->getDesignerScene()->getGridStep().x(), 2), 2, 1000);
	gridStepSizeProp->setPropertyTip("The distance between two grid lines.");
	editorGroup->addProperty(gridStepSizeProp);
	
	PropertyInt* wideStepProp = new PropertyInt("Wide step counter", std::max(m_designer->getView()->getDesignerScene()->getWideGridLineCounter().x(), 1), 1, 1000);
	wideStepProp->setPropertyTip("Every 'x' gridline will be a wide line (e.g. every 10th line).")	;
	editorGroup->addProperty(wideStepProp);
	
	PropertyStringList* gridSnapProp = new PropertyStringList(
		"Grid snap mode", 
		Grid::toString(m_designer->getView()->getDesignerScene()->getGridSnapMode()), 
		std::list<std::string>({ 
			Grid::toString(Grid::NoGridSnap),
			Grid::toString(Grid::SnapTopLeft),
			Grid::toString(Grid::SnapCenter)
		})
	);
	gridSnapProp->setPropertyTip("If enabled items will snap to the grid. This also works if the grid is hidden.");
	editorGroup->addProperty(gridSnapProp);
	editorGroup->addProperty(new PropertyDouble("Width", m_designer->getView()->sceneRect().width()));
	editorGroup->addProperty(new PropertyDouble("Height", m_designer->getView()->sceneRect().height()));

	cfg.addRootGroup(editorGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}
	
void GraphicsItemDesignerNavigationRoot::propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {
	using namespace ot;

	if (_item->getGroupName() == "Editor" && _itemData.getPropertyName() == "Show grid") {
		PropertyInputBool* input = dynamic_cast<PropertyInputBool*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}
		
		m_designer->getView()->getDesignerScene()->setGridFlags((input->isChecked() ? (Grid::ShowNormalLines | Grid::ShowWideLines) : Grid::NoGridFlags));
		m_designer->getView()->update();
	}
	else if (_item->getGroupName() == "Editor" && _itemData.getPropertyName() == "Grid step size") {
		PropertyInputInt* input = dynamic_cast<PropertyInputInt*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}
		m_designer->getView()->getDesignerScene()->setGridStep(input->getValue());
		m_designer->getView()->update();
	}
	else if (_item->getGroupName() == "Editor" && _itemData.getPropertyName() == "Wide step counter") {
		PropertyInputInt* input = dynamic_cast<PropertyInputInt*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}
		m_designer->getView()->getDesignerScene()->setWideGridLineCounter(input->getValue());
		m_designer->getView()->update();
	}
	else if (_item->getGroupName() == "Editor" && _itemData.getPropertyName() == "Grid snap mode") {
		PropertyInputStringList* input = dynamic_cast<PropertyInputStringList*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}
		m_designer->getView()->getDesignerScene()->setGridSnapMode(Grid::stringToGridSnapMode(input->getCurrentText().toStdString()));
	}
	else if (_item->getGroupName() == "Editor" && _itemData.getPropertyName() == "Width") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		m_designer->getView()->setItemSize(QSizeF(input->getValue(), m_designer->getView()->getItemSize().height()));
	}
	else if (_item->getGroupName() == "Editor" && _itemData.getPropertyName() == "Height") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		m_designer->getView()->setItemSize(QSizeF(m_designer->getView()->getItemSize().width(), input->getValue()));
	}
	else {
		OT_LOG_E("Unknown property { \"Group\": \"" + _item->getGroupName() + "\", \"Item\": \"" + _itemData.getPropertyName() + "\" }");
	}
}

void GraphicsItemDesignerNavigationRoot::propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {

}