// @otlicense

#pragma once
#include "SelectionChangedObserver.h"
#include "OTGui/ToolBarButtonCfg.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "OTServiceFoundation/UiComponent.h"
#include "EntityMaterial.h"

class MaterialHandler : public SelectionChangedObserver
{
public:
	MaterialHandler();

	void addButtons(ot::components::UiComponent* _uiComponent);
	static EntityMaterial* createNewMaterial(const std::string& _materialName);
	
private:
	const std::string c_groupMaterial = "Material";
	ot::ToolBarButtonCfg m_buttonCreateMaterial;
	ot::ToolBarButtonCfg m_buttonShowByMaterial;
	ot::ToolBarButtonCfg m_buttonMaterialMissing;

	ot::ButtonHandler m_buttonHandler;
	void handleShowByMaterial();
	void handleShowMaterialMissing();	
	void handleCreateNewMaterial();

	// Inherited via SelectionChangedObserver
	void updatedSelection(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons) override;

};
