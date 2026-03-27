// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/DataStruct/GenericDataStructMatrix.h"

namespace ot
{
	class OT_CORE_API_EXPORT GenericDataStructMatrixMerger
	{
	public:
		static GenericDataStructMatrix tableMerge(const std::list<GenericDataStructMatrix>& _matrices, bool _horizontalHeader = true);
	};

}