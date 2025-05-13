#pragma once
#include "SelectionChangedObserver.h"
#include "OTServiceFoundation/MenuButtonDescription.h"
#include "OTServiceFoundation/UiComponent.h"
#include "ActionAndFunctionHandler.h"
#include "EntityMaterial.h"

class MaterialHandler : public ActionAndFunctionHandler, public SelectionChangedObserver
{
public:
	void addButtons(ot::components::UiComponent* _uiComponent, const std::string& _pageName);
	std::string showByMaterial(ot::JsonDocument& _document);
	std::string showMaterialMissing(ot::JsonDocument& _document);
	std::string createNewMaterial(ot::JsonDocument& _document);
	static EntityMaterial* createNewMaterial(const std::string& _materialName);
	
protected:
	virtual bool handleAction(const std::string& _action, ot::JsonDocument& _doc) override;

private:
	const std::string m_groupMaterial = "Material";
	ot::MenuButtonDescription m_buttonCreateMaterial;
	ot::MenuButtonDescription m_buttonShowByMaterial;
	ot::MenuButtonDescription m_buttonMaterialMissing;

	// Inherited via SelectionChangedObserver
	void updatedSelection(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons) override;

};
