// @otlicense

#pragma once
#include <map>
#include <string>
#include "OTCore/Variable.h"
#include "OTCore/GenericDataStruct.h"

struct PipelineDataDocument
{
	std::map<std::string, ot::Variable> m_parameter;
	std::shared_ptr<ot::GenericDataStruct> m_quantity = nullptr;
};
