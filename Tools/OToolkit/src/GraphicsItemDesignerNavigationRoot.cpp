// @otlicense

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
	editorGroup->addProperty(new PropertyDouble("Width", m_designer->getView()->sceneRect().width(), 10., 2000));
	editorGroup->addProperty(new PropertyDouble("Height", m_designer->getView()->sceneRect().height(), 10., 2000));

	cfg.addRootGroup(editorGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}
	
void GraphicsItemDesignerNavigationRoot::propertyChanged(const ot::Property* const _property) {
	using namespace ot;

	const ot::PropertyGroup* group = _property->getParentGroup();
	if (!group) {
		OT_LOG_EA("Data mismatch");
		return;
	}

	if (group->getName() == "Editor" && _property->getPropertyName() == "Show grid") {
		const PropertyBool* actualProperty = dynamic_cast<const PropertyBool*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}
		
		m_designer->getView()->getDesignerScene()->setGridFlags((actualProperty->getValue() ? (Grid::ShowNormalLines | Grid::ShowWideLines) : Grid::NoGridFlags));
		m_designer->getView()->update();
	}
	else if (group->getName() == "Editor" && _property->getPropertyName() == "Grid step size") {
		const PropertyInt* actualProperty = dynamic_cast<const PropertyInt*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		m_designer->getView()->getDesignerScene()->setGridStep(actualProperty->getValue());
		m_designer->getView()->update();
	}
	else if (group->getName() == "Editor" && _property->getPropertyName() == "Wide step counter") {
		const PropertyInt* actualProperty = dynamic_cast<const PropertyInt*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		m_designer->getView()->getDesignerScene()->setWideGridLineCounter(actualProperty->getValue());
		m_designer->getView()->update();
	}
	else if (group->getName() == "Editor" && _property->getPropertyName() == "Grid snap mode") {
		const PropertyStringList* actualProperty = dynamic_cast<const PropertyStringList*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		m_designer->getView()->getDesignerScene()->setGridSnapMode(Grid::stringToGridSnapMode(actualProperty->getCurrent()));
	}
	else if (group->getName() == "Editor" && _property->getPropertyName() == "Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		m_designer->getView()->setItemSize(QSizeF(actualProperty->getValue(), m_designer->getView()->getItemSize().height()));
	}
	else if (group->getName() == "Editor" && _property->getPropertyName() == "Height") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		m_designer->getView()->setItemSize(QSizeF(m_designer->getView()->getItemSize().width(), actualProperty->getValue()));
	}
	else {
		OT_LOG_E("Unknown property { \"Group\": \"" + group->getName() + "\", \"Item\": \"" + _property->getPropertyName() + "\" }");
	}
}

void GraphicsItemDesignerNavigationRoot::propertyDeleteRequested(const ot::Property* const _property) {

}