// @otlicense

#pragma once

// Open twin header
#include "OTServiceFoundation/ApplicationBase.h"		// Base class
#include "OTGui/ToolBarButtonCfg.h"

// C++ header
#include <list>
#include <mutex>
#include <string>

#include "TableHandler.h"
#include "DataCategorizationHandler.h"
#include "TabledataToResultdataHandler.h"
#include "TouchstoneToResultdata.h"
#include "ResultCollectionMetadataAccess.h"
#include "RangeSelectionVisualisationHandler.h"
#include "BatchedCategorisationHandler.h"


#include "OTCore/FolderNames.h"
#include "OTServiceFoundation/UILockWrapper.h"

// Forward declaration
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

namespace std {
	class thread;
}

class Application : public ot::ApplicationBase {
public:
	static Application * instance(void);
	static void deleteInstance(void);

	// ##################################################################################################################################

	// Required functions

	//! @brief Will be called whenever a action should be processed. Core actions will be processed in the base and will not be forwarded to this function (see documentation)
	//! @param _action The action that should be processed
	//! @param _doc The document containing all the information
	virtual std::string processAction(const std::string & _action, ot::JsonDocument& _doc) override;

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	//! @brief Will be called when a model service connected to the session and is ready to work
	virtual void modelConnected(ot::components::ModelComponent * _model) override;

	// ##################################################################################################################################

	// Add your custom functions/ members here

	//! A handler can be created to handle the the specified action
	//! In this example the first parameter is the name of the callback function
	//! The second parameter is the class name where the handler is created at
	//! The last parameter is a C or C++ String containing the action name
	//OT_HANDLER(myHandleFunctionName, Application, "actionToHandle");

	virtual void modelSelectionChanged(void) override;
	virtual ~Application();

private:
	Application();

	std::mutex m_onlyOneActionPerTime;
	ot::UILockWrapper* m_twoPartsAction = nullptr;

	ot::ToolBarButtonCfg m_buttonImportTouchstone;
	
	ot::ToolBarButtonCfg m_buttonCreateMSMDEntry;
	ot::ToolBarButtonCfg m_buttonCreateRMDEntry;
	ot::ToolBarButtonCfg m_buttonCreateParameterEntry;
	ot::ToolBarButtonCfg m_buttonCreateQuantityEntry;
	ot::ToolBarButtonCfg m_buttonLockCharacterisation;
	ot::ToolBarButtonCfg m_buttonUnLockCharacterisation;

	ot::ToolBarButtonCfg m_buttonAutomaticCreationMSMD;
	ot::ToolBarButtonCfg m_buttonAddBatchCreator;

	ot::ToolBarButtonCfg m_buttonCreateDataCollection;

	const std::string _dataSourcesFolder = "Files";
	const std::string _scriptsFolder = ot::FolderNames::PythonScriptFolder;
	const std::string _tableFolder = "Tables";
	
	const std::string _previewTableNAme = "Preview";
	const std::string _datasetFolder = ot::FolderNames::DatasetFolder;

	ot::UID _visualizationModel = -1;
	
	std::atomic_bool m_selectionWorkerRunning;
	std::thread* m_selectionWorker;
	std::mutex m_selectedEntitiesMutex;
	std::list<ot::UIDList> m_selectedEntitiesQueue;

	TableHandler* _tableHandler = nullptr;
	DataCategorizationHandler* m_parametrizedDataHandler = nullptr;
	TabledataToResultdataHandler* _tabledataToResultdataHandler = nullptr;
	TouchstoneToResultdata* _touchstoneToResultdata = nullptr;
	
	RangeSelectionVisualisationHandler m_rangleSelectionVisualisationHandler;
	BatchedCategorisationHandler m_batchedCategorisationHandler;
	void HandleSelectionChanged();
	void ProcessActionDetached(const std::string& _action, ot::JsonDocument _doc, ot::UIDList _selectedEntities);
};
