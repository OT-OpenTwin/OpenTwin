#include "stdafx.h"

#include "UIControlsHandler.h"
#include "OTServiceFoundation/UiComponent.h"
#include "TemplateDefaultManager.h"
#include "OTGui/KeySequence.h"

UIControlsHandler::UIControlsHandler()
{
	if (Application::instance()->isUiConnected())
	{
		// Load the template defaults if any
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults("UI Configuration");
		createPages();
		createGroups();
		setupButtons();
		createButtons();
	}
}

void UIControlsHandler::createPages()
{
	ot::components::UiComponent* uiComponent = Application::instance()->uiComponent();
	assert(uiComponent != nullptr);
	uiComponent->addMenuPage(m_pageModel);
	uiComponent->addMenuPage("View");
}

void UIControlsHandler::createGroups()
{
	ot::components::UiComponent* uiComponent = Application::instance()->uiComponent();
	assert(uiComponent != nullptr);

	uiComponent->addMenuGroup(m_pageModel, m_groupEdit);
	uiComponent->addMenuGroup(m_pageModel, m_groupGeometry);
	uiComponent->addMenuGroup(m_pageModel, m_groupImport);
	uiComponent->addMenuGroup(m_pageModel, m_groupMaterial);
	uiComponent->addMenuGroup(m_pageModel, m_groupParameters);
	uiComponent->addMenuGroup(m_pageModel, m_groupPlot);
}

void UIControlsHandler::setupButtons()
{
	m_buttonGeometryInfo.SetDescription(m_pageModel, m_groupGeometry,"Info", "Info");
	
	m_buttonCreateMaterial.SetDescription(m_pageModel,m_groupMaterial, "Create Material", "Create Material");
	m_buttonMaterialMissing.SetDescription(m_pageModel, m_groupMaterial, "Show By Material", "Show By Material");
	m_buttonShowByMaterial.SetDescription(m_pageModel, m_groupMaterial, "Material Missing", "Material Missing");

	m_buttonCreateParameter.SetDescription(m_pageModel,m_groupParameters, "Create Parameter", "Create Parameter");
	
	m_buttonDelete.SetDescription(m_pageModel, m_groupEdit, "Delete", "Delete");
	m_buttonRedo.SetDescription(m_pageModel, m_groupEdit, "Redo", "Redo");
	m_buttonUndo.SetDescription(m_pageModel, m_groupEdit, "Undo", "Undo");
	
	m_buttonAddCurves.SetDescription(m_pageModel,m_groupPlot, "Add Curves", "Add Curves");
}

void UIControlsHandler::createButtons()
{
	ot::components::UiComponent* uiComponent = Application::instance()->uiComponent();
	assert(uiComponent != nullptr);
	
	ot::LockTypeFlags modelWrite(ot::LockModelWrite);
	ot::LockTypeFlags modelRead (ot::LockModelRead);

	uiComponent->addMenuButton(m_buttonGeometryInfo,modelRead,"Information","Default", ot::KeySequence(ot::Key_Control, ot::Key_I));
	
	uiComponent->addMenuButton(m_buttonCreateMaterial,modelWrite,"AddMaterial");
	uiComponent->addMenuButton(m_buttonShowByMaterial,modelRead,"ShowByMaterial");
	uiComponent->addMenuButton(m_buttonMaterialMissing,modelRead, "ShowMaterialMissing");

	uiComponent->addMenuButton(m_buttonCreateParameter,modelRead, "CreateParameter");

	uiComponent->addMenuButton(m_buttonUndo,modelWrite,"Undo","Default", ot::KeySequence(ot::Key_Control, ot::Key_Z));
	uiComponent->addMenuButton(m_buttonRedo,modelWrite,"Redo", "Default", ot::KeySequence(ot::Key_Control, ot::Key_Y));
	uiComponent->addMenuButton(m_buttonDelete,modelWrite,"Delete", "Default", ot::KeySequence(ot::Key_Delete));

	uiComponent->addMenuButton(m_buttonAddCurves,modelWrite,"Result1DVisible");
}
