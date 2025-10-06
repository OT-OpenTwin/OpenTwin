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
	ot::components::UiComponent* uiComponent = Application::instance()->getUiComponent();
	assert(uiComponent != nullptr);
	uiComponent->addMenuPage(m_pageModel);
	uiComponent->addMenuPage("View");
}

void UIControlsHandler::createGroups()
{
	ot::components::UiComponent* uiComponent = Application::instance()->getUiComponent();
	assert(uiComponent != nullptr);

	uiComponent->addMenuGroup(m_pageModel, m_groupEdit);
	uiComponent->addMenuGroup(m_pageModel, m_groupGeometry);
	uiComponent->addMenuGroup(m_pageModel, m_groupImport);
	
	uiComponent->addMenuGroup(m_pageModel, m_groupParameters);
}

void UIControlsHandler::setupButtons()
{
	m_buttonGeometryInfo = ot::ToolBarButtonCfg(m_pageModel, m_groupGeometry, "Info", "Default/Information");
	m_buttonGeometryInfo.setButtonLockFlags(ot::LockModelRead);
	m_buttonGeometryInfo.setButtonKeySequence(ot::KeySequence(ot::Key_Control, ot::Key_I));
	
	m_buttonCreateParameter = ot::ToolBarButtonCfg(m_pageModel, m_groupParameters, "Create Parameter", "Default/CreateParameter");
	m_buttonCreateParameter.setButtonLockFlags(ot::LockModelRead);
	
	m_buttonDelete = ot::ToolBarButtonCfg(m_pageModel, m_groupEdit, "Delete", "Default/Delete");
	m_buttonDelete.setButtonLockFlags(ot::LockModelWrite);
	m_buttonDelete.setButtonKeySequence(ot::KeySequence(ot::Key_Delete));

	m_buttonRedo = ot::ToolBarButtonCfg(m_pageModel, m_groupEdit, "Redo", "Default/Redo");
	m_buttonRedo.setButtonLockFlags(ot::LockModelWrite);
	m_buttonRedo.setButtonKeySequence(ot::KeySequence(ot::Key_Control, ot::Key_Y));

	m_buttonUndo = ot::ToolBarButtonCfg(m_pageModel, m_groupEdit, "Undo", "Default/Undo");
	m_buttonUndo.setButtonLockFlags(ot::LockModelWrite);
	m_buttonUndo.setButtonKeySequence(ot::KeySequence(ot::Key_Control, ot::Key_Z));
}

void UIControlsHandler::createButtons()
{
	ot::components::UiComponent* uiComponent = Application::instance()->getUiComponent();
	assert(uiComponent != nullptr);
	
	uiComponent->addMenuButton(m_buttonGeometryInfo);

	uiComponent->addMenuButton(m_buttonCreateParameter);

	uiComponent->addMenuButton(m_buttonUndo);
	uiComponent->addMenuButton(m_buttonRedo);
	uiComponent->addMenuButton(m_buttonDelete);
}
