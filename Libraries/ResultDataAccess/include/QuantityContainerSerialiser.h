// @otlicense
// File: QuantityContainerSerialiser.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once
#include <stdint.h>
#include "ResultCollectionExtender.h"
#include "ResultImportLogger.h"

#include "OTCore/CoreTypes.h"

#include "QuantityDescription.h"
#include "QuantityDescriptionCurve.h"
#include "QuantityDescriptionCurveComplex.h"
#include "QuantityDescriptionSParameter.h"
#include "QuantityDescriptionMatrix.h"

class QuantityContainerSerialiser
{
public:

	QuantityContainerSerialiser(const std::string& _collectionName, ResultImportLogger& _logger);
	
	void storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, const uint64_t _numberOfParameterValues, QuantityDescription* _quantityDescription);
	void flushQuantityContainer();

private:
	DataStorageAPI::ResultDataStorageAPI m_dataStorageAccess;
	void storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, const uint64_t _numberOfParameterValues, QuantityDescriptionCurve* _quantityDescription);
	void storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, const uint64_t _numberOfParameterValues, QuantityDescriptionCurveComplex* _quantityDescription);
	void storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, const uint64_t _numberOfParameterValues, QuantityDescriptionSParameter* _quantityDescription);
	void storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, const uint64_t _numberOfParameterValues, QuantityDescriptionMatrix* _quantityDescription);

	ResultImportLogger& m_logger;
	uint64_t m_bucketSize = 1;
	const uint32_t m_bufferSize = 50;

	std::vector<QuantityContainer> m_quantityContainer;

	void addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>&& _parameterValues, uint64_t _quantityIndex, const ot::Variable& _quantityValue);
	void addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _parameterValues, uint64_t _quantityIndex, const ot::Variable& _quantityValue);

};
