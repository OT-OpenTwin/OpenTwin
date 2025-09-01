#pragma once
#include "OTServiceFoundation/PythonServiceInterface.h"
#include "EntityTableSelectedRanges.h"
#include "BusinessLogicHandler.h"
#include "BatchUpdateInformation.h"

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

	void run(const std::string& _seriesNameBase, bool _lastRun);

	inline void ensureEssentials();
	std::list<std::shared_ptr<EntityTableSelectedRanges>> findAllTableSelectionsWithConsiderForBatching();
	std::map<uint32_t, std::list<BatchUpdateInformation>> createNewMSMDWithSelections(std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>>& _allRelevantTableSelectionsByMSMD, const std::string& _newMSMDNameBase, bool _lastRun);
	std::map<std::string, ot::UID> getTableUIDByNames(std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>>& _allRelevantTableSelectionsByMSMD);
	std::map<std::string, ot::UID> getPythonScriptUIDByNames(std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>>& _allRelevantTableSelectionsByMSMD);
};
