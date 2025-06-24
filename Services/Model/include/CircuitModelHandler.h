#pragma once

// OpenTwin header
#include "OTServiceFoundation/MenuButtonDescription.h"
#include "OTServiceFoundation/UiComponent.h"
#include "ActionAndFunctionHandler.h"
#include "IVisualisationText.h"
#include "IVisualisationTable.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTGui/TableCfg.h"
#include "OTCore/GenericDataStructMatrix.h"

// Service header
#include "LibraryManagementWrapper.h"

class CircuitModelHandler : public ActionAndFunctionHandler, public BusinessLogicHandler {
public:
	CircuitModelHandler() = default;
	virtual ~CircuitModelHandler() = default;

	CircuitModelHandler(const CircuitModelHandler& _other) = delete;
	CircuitModelHandler(CircuitModelHandler&& _other) = delete;
	CircuitModelHandler& operator=(const CircuitModelHandler& _other) = delete;
	CircuitModelHandler& operator=(CircuitModelHandler&& _other) = delete;

	void addButtons(ot::components::UiComponent* _uiComponent, const std::string& _pageName);


protected:
	virtual bool handleAction(const std::string& _action, ot::JsonDocument& _doc) override;

private:

	ot::UIDList m_entityIDsTopo;
	ot::UIDList m_entityVersionsTopo;
	ot::UIDList m_entityIDsData {};
	ot::UIDList m_entityVersionsData {};
	std::list<bool> m_forceVisible;

	void createModelDialog();
	void createModelTextEntity(std::string _modelName);
	std::string sendRequestOfDialogConfig(ot::JsonDocument& _doc);
	ot::MenuButtonDescription m_buttonAddCircuitModel;
	LibraryManagementWrapper m_libraryMangementWrapper;






};
