#include "QuantityContainerCreator.h"

QuantityContainerCreator::QuantityContainerCreator(int32_t& msmdIndex, std::set<std::string>& parameterAbbreviations, int32_t containerSize)
	: _msmdIndex(msmdIndex), _parameterAbbreviations(parameterAbbreviations), _containerSize(containerSize), _isFlatCollection(_containerSize == 1)
{
	
}

void QuantityContainerCreator::AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, ot::Variable value)
{
	bool doesAlreadyExist = false;
	//for (auto& quantityContainer : quantityContainerOfType)
	//{
	//	if (quantityContainer.ParameterValueIndicesAndQuantityIndexAreMatching(parameterValueIndices, quantityIndex))
	//	{
	//		doesAlreadyExist = true;
	//	}
	//}
	if (!doesAlreadyExist)
	{
		//if (quantityContainerOfType.back().GetValueArraySize() >= _containerSize)
		//{
		//	assert(0);// Container size should match with the number of parameter apperiences.
		//}
		_quantContainers.push_back(QuantityContainer(_msmdIndex, _parameterAbbreviations, parameterValueIndices, quantityIndex, _isFlatCollection));
		_quantContainers.back().AddValue(value);
	}

}

void QuantityContainerCreator::Flush(DataStorageAPI::ResultDataStorageAPI& storageAPI)
{
	uint64_t totalSize = _quantContainers.size();
	if (totalSize > 0)
	{
		_updater->SetTotalNumberOfUpdates(5, totalSize);
		_triggerCounter = 0;

		for (auto& qc : _quantContainers)
		{
			DataStorageAPI::DataStorageResponse response = storageAPI.InsertDocumentToResultStorage(qc._mongoDocument, _checkForDocumentExistenceBeforeInsert, _quequeDocumentsWhenInserting);
			if (!response.getSuccess())
			{
				throw std::exception("Insertion of quantity container failed.");
			}
			if (_updater != nullptr)
			{
				_triggerCounter++;
				_updater->TriggerUpdate(_triggerCounter);
			}
		}
		_quantContainers.clear();
	}
}

void QuantityContainerCreator::SetUpdateProgress(ProgressUpdater& updater)
{
	_updater = &updater;
}
