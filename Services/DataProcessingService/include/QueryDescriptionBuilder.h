#pragma once
#include "OTResultDataAccess/ResultCollection/ResultCollectionMetadataAccess.h"
#include "OTCore/QueryDescription/QueryDescription.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
class QueryDescriptionBuilder
{
public:
	static ot::QueryDescription create(const ot::ValueComparisonDescription& _valueDescription, const MetadataParameter* _parameter);
	static ot::QueryDescription create(const ot::ValueComparisonDescription& _valueDescription, const MetadataQuantity* _quantity);
	static ot::QueryDescription create(const MetadataSeries* _series);
};
