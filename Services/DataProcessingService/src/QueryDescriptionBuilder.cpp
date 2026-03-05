#include "QueryDescriptionBuilder.h"

ot::QueryDescription QueryDescriptionBuilder::create(const ot::ValueComparisonDescription& _valueDescription, const MetadataParameter* _parameter)
{	
	ot::QueryDescription queryDescription;
	queryDescription.setComparisonDescription(_valueDescription);

	ot::QueryTargetDescription queryTargetDescription;
	
	TupleInstance parameterTuple;
	parameterTuple.setTupleUnits({ _parameter->unit });
	parameterTuple.setTupleElementDataTypes({ _parameter->typeName });
	parameterTuple.setTupleFormatName("");
	parameterTuple.setTupleTypeName("");
	queryTargetDescription.setTupleInstance(parameterTuple);
		
	queryTargetDescription.setTargetLabel(_parameter->parameterLabel);
	queryTargetDescription.setMongoDBFieldName(std::to_string(_parameter->parameterUID));
	
	queryDescription.setQueryTargetDescription(queryTargetDescription);
	return queryDescription;
}

ot::QueryDescription QueryDescriptionBuilder::create(const ot::ValueComparisonDescription& _valueDescription, const MetadataQuantity* _quantity)
{
	ot::QueryDescription queryDescription;
	queryDescription.setComparisonDescription(_valueDescription);
	
	ot::QueryTargetDescription queryTargetDescription;
	queryTargetDescription.setTupleInstance(_quantity->m_tupleDescription);
	queryTargetDescription.setTargetLabel(_quantity->quantityLabel);
	queryTargetDescription.setMongoDBFieldName(MetadataQuantity::getFieldName());
	queryDescription.setQueryTargetDescription(queryTargetDescription);

	return queryDescription;
}

ot::QueryDescription QueryDescriptionBuilder::create(const ot::ValueComparisonDescription& _valueDescription, const MetadataSeries* _series)
{
	ot::QueryDescription queryDescription;
	// Simply a Series = id
	queryDescription.setComparisonDescription(_valueDescription);

	ot::QueryTargetDescription queryTargetDescription;

	TupleInstance seriesTuple;
	
	seriesTuple.setTupleUnits({ ""});
	seriesTuple.setTupleElementDataTypes({ ot::TypeNames::getInt64TypeName()});
	seriesTuple.setTupleFormatName("");
	seriesTuple.setTupleTypeName("");
	queryTargetDescription.setTupleInstance(seriesTuple);

	queryTargetDescription.setTargetLabel(_series->getLabel());
	queryTargetDescription.setMongoDBFieldName(MetadataSeries::getFieldName());

	queryDescription.setQueryTargetDescription(queryTargetDescription);
	return queryDescription;
}
