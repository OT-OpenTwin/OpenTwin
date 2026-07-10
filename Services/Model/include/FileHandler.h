#pragma once

// OpenTwin header
#include "OTModelEntities/Visualization/IVisualisationText.h"
#include "OTModelEntities/Visualization/IVisualisationTable.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/DataStruct/GenericDataStructMatrix.h"
#include "OTGui/Widgets/TableCfg.h"
#include "OTGui/Widgets/ToolBarButtonCfg.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "OTGuiAPI/TableActionHandler.h"
#include "OTGuiAPI/TextEditorActionHandler.h"
#include "OTCommunication/Handler/ActionHandler.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTModelEntities/EntityFileText.h"
#include "OTModelEntities/EntityPythonManifest.h"
#include "OTModelEntities/Lms/UserLibraryElement.h"
#include "OTGui/Dialog/PropertyDialogCfg.h"

class FileHandler : public BusinessLogicHandler, public ot::TextEditorActionHandler, public ot::TableActionHandler
{
	OT_DECL_NOCOPY(FileHandler)
		OT_DECL_NOMOVE(FileHandler)
public:
	FileHandler();
	virtual ~FileHandler() = default;

	void addButtons(ot::components::UiComponent* _uiComponent);
	void handleOverwriteResponse(const std::string& _filePath, bool _overwrite);

	//! @brief Handles the export file dialog and performs either local or user library export.
	//! @param _dialogCfg The dialog configuration containing the selected entities
	//! @param _exportToUserLibrary true for user library export, false for local library export
	void handleExportFileDialog(const ot::PropertyDialogCfg& _dialogCfg, bool _exportToUserLibrary);

	const std::string m_pythonScriptDialogGroup = "Python Script Export";
	const std::string m_circuitModelDialogGroup = "Circuit Model Export";
private:
	const std::string c_groupName = "File Imports";
	const std::string m_pythonUserCollectionName = "PythonScripts_User";
	const std::string m_circuitModelUserCollectionName = "CircuitData_User";
	const std::string m_environmentUserCollectionName = "Environments_User";
	ot::ToolBarButtonCfg m_buttonFileImport;
	ot::ToolBarButtonCfg m_buttonPythonImport;
	ot::ToolBarButtonCfg m_buttonExportFileToLibrary;
	ot::ToolBarButtonCfg m_buttonExportToUserLibrary;

	ot::UIDList m_entityIDsTopo;
	ot::UIDList m_entityVersionsTopo;
	ot::UIDList m_entityIDsData;
	ot::UIDList m_entityVersionsData;
	std::list<bool> m_forceVisible;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Button Handler

	ot::ButtonHandler m_buttonHandler;
	void handleImportTextFileButton();
	void handleImportPythonScriptButton();
	void handleExportFilesToLibrary();
	void handleExportToUserLibrary();

	//! @brief Shows a dialog to select a file for export to either local directory or user database collection.
	void showExportDialog(const std::string& _title, const std::string& _callbackAction, bool _isLocalExport);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action Handler

	ot::ActionHandler m_actionHandler;
	void handleImportTextFile(ot::JsonDocument& _document);
	void handleImportPythonScript(ot::JsonDocument& _document);
	virtual ot::ReturnMessage textEditorSaveRequested(const std::string& _entityName, const std::string& _text, size_t _nextChunkStartIndex) override;
	virtual ot::ReturnMessage tableSaveRequested(const ot::TableCfg& _cfg) override;
	virtual ot::ReturnMessage tableColumnFilterChanged(const ot::TableFilterChangeEvent& _event) override;

	ot::ReturnMessage handleRequestTextData(ot::JsonDocument& _document);

	constexpr static std::string_view c_promptActionOverwriteFile = "Model.Prompt.OverwriteFile";
	void handleOverwriteFilePromptResponse(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Helper

	void importFile(const std::string& _fileMask, const std::string& _dialogTitle, const std::string& _functionName);
	void storeTextFile(ot::JsonDocument&& _doc, const std::string& _folderName);
	void addTextFilesToModel();
	void clearBuffer();

	void storeChangedText(ot::IVisualisationText* _entity, const std::string _text, size_t _nextChunkStartIndex);
	void storeChangedTable(ot::IVisualisationTable* _entity, const ot::TableCfg& _cfg);

	void processTableColumnFilterChanged(const ot::TableFilterChangeEvent& _event, ot::IVisualisationTable* _entity);

	void NotifyOwnerAsync(ot::JsonDocument&& _doc, const std::string _owner);
	void storeFileInDataBase(const std::string& _text, const std::string& _fileName, std::list<std::string>& _folderContent, const std::string& _folderName, const std::string& _fileFilter);

	// ###########################################################################################################################################################################################################################################################################################################################
	
	// Export to library functions

	struct DialogExportEntities {
		EntityFileText* circuitModel = nullptr;
		EntityFileText* circuitMetaFile = nullptr;
		EntityFileText* pythonScript = nullptr;
		EntityPythonManifest* pythonManifest = nullptr;
		EntityFileText* pythonMetaFile = nullptr;
		EntityFileText* manifestMetaFile = nullptr;
		bool isPythonExport = false;
		bool isCircuitExport = false;
	};

	enum class FileOverwriteStatus {
		Write,
		Skip,
		PromptUser
	};

	struct PendingScriptExport {
		std::string contentFilePath;
		std::string contentData;
		std::string metaFilePath;
		std::string metaContent;
		FileOverwriteStatus status = FileOverwriteStatus::Skip;
	};

	struct PendingFileOverwrite {
		std::string contentFilePath;
		std::string contentNewContent;
		std::string metaFilePath;
		std::string metaNewContent;

		bool hasLinkedScriptExport = false;
		PendingScriptExport linkedScriptExport;
	};

	std::map<std::string, PendingFileOverwrite> m_pendingFileOverwrites;

	struct ExportResult {
		FileOverwriteStatus status;
		ot::UID libraryElementID;
		std::string contentFilePath;
	};

	DialogExportEntities loadDialogEntities(const ot::PropertyDialogCfg& _dialogCfg, bool _isUserExport);
	EntityBase* loadEntityByPath(const std::string& _entityPath, const std::string& _description);

	void handleCircuitExport(const DialogExportEntities& _entities, bool _exportToUserLibrary, const ot::PropertyDialogCfg& _dialogCfg);
	void handlePythonExport(const DialogExportEntities& _entities, bool _exportToUserLibrary, const ot::PropertyDialogCfg& _dialogCfg);

	bool validateMetaDataFile(EntityFileText* _metaFile);
	std::string getLibraryDataPath() const;
	bool ensureDirectoryExists(const std::string& _path) const;
	void writeFileToPath(const std::string& _filePath, const std::string& _content) const;
	std::string ensureFileExtension(const std::string& _fileName, const std::string& _extension) const;

	void exportPythonScriptsToLibrary(ot::UID _scriptID, ot::UID _manifestID, ot::UID _pythonMetaID, ot::UID _manifestMetaID, ot::UID _environmentID);
	ExportResult exportPythonManifest(EntityPythonManifest* _manifestEntity, EntityFileText* _metaEntity, const std::string& _basePath, ot::UID _environmentID);
	void exportPythonScript(EntityFileText* _scriptEntity, EntityFileText* _metaEntity, const std::string& _basePath, ot::UID _environmentID);
	void exportCircuitModel(EntityFileText* _modelEntity, EntityFileText* _metaEntity, const std::string& _basePath);

	FileOverwriteStatus checkAndHandleFileOverwrite(const std::string& _filePath, const std::string& _newContent, const std::string& _metaFilePath, const std::string& _metaNewContent) const;
	void promptUserForOverwrite(const std::string& _contentFilePath, const std::string& _metaFilePath, const std::string& _contentNewContent, const std::string& _metaNewContent) const;
	std::string createIncrementedPath(const std::string& _filePath);

	ot::UID readLibraryElementIDFromMetaFile(const std::string& _metaFilePath) const;
	PendingScriptExport prepareScriptExportData(EntityFileText* _scriptEntity, EntityFileText* _metaEntity, const std::string& _basePath, ot::UID _environmentID);
	void processLinkedScriptExport(const PendingScriptExport& _scriptExport, ot::UID _manifestLibraryElementID);
	std::string updateDependencyIDInMetaContent(const std::string& _metaContent, ot::UID _newDependencyID);
	void updateExistingMetaFileDependencyID(const std::string& _metaFilePath, ot::UID _newDependencyID);

	void createLibraryElementsForCircuitModel(EntityFileText* _modelEntity, const ot::PropertyDialogCfg& _dialogCfg, std::list<ot::UserLibraryElement>& _elementsToExport);
	void createLibraryElementsForPythonScript(EntityFileText* _scriptEntity, ot::UID _environmentID, const ot::PropertyDialogCfg& _dialogCfg, std::list<ot::UserLibraryElement>& _elementsToExport);
	void createLibraryElementsForPythonManifest(EntityPythonManifest* _manifestEntity, const ot::PropertyDialogCfg& _dialogCfg, std::list<ot::UserLibraryElement>& _elementsToExport);
	void exportLibraryElementsToUserLibrary(const std::list<ot::UserLibraryElement>& _elementsToExport);
};