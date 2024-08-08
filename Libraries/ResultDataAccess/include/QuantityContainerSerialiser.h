#pragma once
#include <stdint.h>
#include "ResultCollectionExtender.h"
#include "OTCore/CoreTypes.h"

#include "QuantityDescription.h"
#include "QuantityDescriptionCurve.h"
#include "QuantityDescriptionCurveComplex.h"
#include "QuantityDescriptionSParameter.h"

class QuantityContainerSerialiser
{
public:

	QuantityContainerSerialiser(const std::string& _collectionName);
	
	void storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, const uint64_t _numberOfParameterValues, QuantityDescription* _quantityDescription);
	void flushQuantityContainer();

private:
	DataStorageAPI::ResultDataStorageAPI m_dataStorageAccess;
	void storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, const uint64_t _numberOfParameterValues, QuantityDescriptionCurve* _quantityDescription);
	void storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, const uint64_t _numberOfParameterValues, QuantityDescriptionCurveComplex* _quantityDescription);
	void storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, const uint64_t _numberOfParameterValues, QuantityDescriptionSParameter* _quantityDescription);

	uint64_t m_bucketSize = 1;
	const uint32_t m_bufferSize = 50;

	std::vector<QuantityContainer> m_quantityContainer;

	void addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>&& _parameterValues, uint64_t _quantityIndex, const ot::Variable& _quantityValue);
	void addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _parameterValues, uint64_t _quantityIndex, const ot::Variable& _quantityValue);

};
