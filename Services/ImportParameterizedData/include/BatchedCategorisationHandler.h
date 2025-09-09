#pragma once
#include "OTServiceFoundation/PythonServiceInterface.h"
#include "EntityTableSelectedRanges.h"
#include "BusinessLogicHandler.h"
#include "BatchUpdateInformation.h"
#include "UniqueEntityNameCreator.h"

#include <memory>
#include <list>

class BatchedCategorisationHandler : public BusinessLogicHandler
{
public:
	BatchedCategorisationHandler() = default;
	~BatchedCategorisationHandler();
	void createNewScriptDescribedMSMD(std::list<ot::UID> _selectedEntities);
	void addCreator();
	
	

private:
	ot::PythonServiceInterface* m_pythonInterface = nullptr;
	std::string m_rmdEntityName;
	ot::UID m_scriptFolderUID = -1;
	UniqueEntityNameCreator m_uniqueEntityNameCreator;
	std::list<std::shared_ptr<EntityTableSelectedRanges>> m_consideredRanges;

	std::list<std::shared_ptr<EntityTableSelectedRanges>> findAllTableSelectionsWithConsiderForBatching();
	void run(bool _lastRun, std::map<std::string, ot::UID>& _tableUIDsByName, std::map<std::string, ot::UID>& _pythonScriptsByName);

	inline void ensureEssentials();
	std::map<uint32_t, std::list<BatchUpdateInformation>> createNewMSMDWithSelections(std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>>& _allRelevantTableSelectionsByMSMD, bool _lastRun, std::map<std::string, ot::UID>& _tableUIDsByName, std::map<std::string, ot::UID>& _pythonScriptsByName);
	std::map<std::string, ot::UID> getAllTableUIDsByName();
	std::map<std::string, ot::UID> getAllPythonScriptUIDsByName();
};
