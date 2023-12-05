#pragma once
#include <stdint.h>
#include <set>
#include <string>
#include <list>

#include "QuantityContainer.h"
#include "ResultDataStorageAPI.h"
#include "ProgressUpdater.h"
#include "OTCore/Variable.h"

class QuantityContainerCreator
{
public:
	QuantityContainerCreator(int32_t& msmdIndex, std::set<std::string>& parameterAbbreviations, int32_t containerSize);
	
	void AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, ot::Variable value);

	void Flush(DataStorageAPI::ResultDataStorageAPI& storageAPI);
	void SetUpdateProgress(ProgressUpdater& updater);

private:
	std::list<QuantityContainer> _quantContainers;
	
	ProgressUpdater* _updater = nullptr;
	int32_t _triggerCounter = 0;

	int32_t _msmdIndex;
	int32_t _containerSize;
	bool _isFlatCollection;
	bool _checkForDocumentExistenceBeforeInsert = false;
	bool _quequeDocumentsWhenInserting = false;
	std::set<std::string> _parameterAbbreviations;
};
