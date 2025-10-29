// @otlicense

#pragma once
#include "OTCore/GenericDataStruct.h"
#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "PipelineData.h"
#include "OTCore/VariableToStringConverter.h"

namespace ot
{
	std::string toString(const ot::GenericDataStruct* _dataStruct);


	std::string toString(const PipelineDataDocument& _document);
}