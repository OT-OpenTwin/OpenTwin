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
	queryTargetDescription.setMongoDBFieldName(std::to_string( _quantity->quantityIndex));
	queryDescription.setQueryTargetDescription(queryTargetDescription);

	return queryDescription;
}

ot::QueryDescription QueryDescriptionBuilder::create(const MetadataSeries* _series)
{
	// Simply a Series = id
	TupleInstance seriesTuple;
	seriesTuple.setTupleUnits({""});
	seriesTuple.setTupleElementDataTypes({ ot::TypeNames::getInt64TypeName()});
	seriesTuple.setTupleFormatName("");
	seriesTuple.setTupleTypeName("");
	
	ot::QueryDescription queryDescription;
	ot::ValueComparisonDescription valueDescription(MetadataSeries::getFieldName(), "=", std::to_string(_series->getSeriesIndex()), seriesTuple);
	queryDescription.setComparisonDescription(valueDescription);

	ot::QueryTargetDescription queryTargetDescription;
	queryTargetDescription.setTupleInstance(seriesTuple);
	queryTargetDescription.setTargetLabel(_series->getLabel());
	queryTargetDescription.setMongoDBFieldName(MetadataSeries::getFieldName());
	queryDescription.setQueryTargetDescription(queryTargetDescription);

	return queryDescription;
}
