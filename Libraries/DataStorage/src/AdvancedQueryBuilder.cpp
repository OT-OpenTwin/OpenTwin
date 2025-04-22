#include "stdafx.h"
#include "AdvancedQueryBuilder.h"
#include "OTCore/VariableToBSONStringConverter.h"
#include "OTCore/ExplicitStringValueConverter.h"
#include <cassert>
#include <stdarg.h>

BsonViewOrValue AdvancedQueryBuilder::createComparison(const std::string& comparator, const ot::Variable& variable)
{
	auto mongoComparator = m_mongoDBComparators.find(comparator);
	if (mongoComparator == m_mongoDBComparators.end() || mongoComparator->second == "$in" || mongoComparator->second == "$nin")
	{
		assert(0);
		throw std::exception("Not supported comparator selected for comparison query.");
	}
	else
	{
		return GenerateFilterQuery(mongoComparator->second, variable);
	}
}

BsonViewOrValue AdvancedQueryBuilder::connectWithAND(std::list<BsonViewOrValue>&& values)
{
	const std::string mongoOperator = "$and";
	return BuildBsonArray(mongoOperator, std::move(values));
}

BsonViewOrValue AdvancedQueryBuilder::connectWithOR(std::list<BsonViewOrValue>&& values)
{
	const std::string mongoOperator = "$or";
	return BuildBsonArray(mongoOperator, std::move(values));
}

BsonViewOrValue AdvancedQueryBuilder::buildRangeQuery(const ValueComparisionDefinition& _definition)
{
	const std::string& name = _definition.getName();
	std::string valueStr = _definition.getValue();
	const std::string& type = _definition.getType();

	valueStr.erase(std::remove(valueStr.begin(), valueStr.end(), ' '), valueStr.end());
	char openingBracket = valueStr[0];
	char closingBracket = valueStr[valueStr.size() - 1];
	valueStr.erase(valueStr.begin());
	valueStr.erase(valueStr.end() - 1);
	size_t posDelimiter = valueStr.find(",");
	const bool notationIsCorrect = (openingBracket == '(' || openingBracket == '[') && (closingBracket == ')' || closingBracket == ']') && posDelimiter != std::string::npos;

	if (notationIsCorrect) {
		const bool dataTypeCompatible = type == ot::TypeNames::getInt32TypeName() || type == ot::TypeNames::getInt64TypeName() || type == ot::TypeNames::getDoubleTypeName() || type == ot::TypeNames::getFloatTypeName();
		if (dataTypeCompatible) {
			const std::string firstValue = valueStr.substr(0, posDelimiter);
			const std::string secondValue = valueStr.substr(posDelimiter + 1);

			ot::Variable vFirstValue = ot::ExplicitStringValueConverter::setValueFromString(firstValue, type);
			ot::Variable vSecondValue = ot::ExplicitStringValueConverter::setValueFromString(secondValue, type);

			std::string correspondingComparator;
			if (openingBracket == '(') {
				correspondingComparator = ">";
			}
			else {
				correspondingComparator = ">=";
			}

			auto firstCompare = createComparison(correspondingComparator, vFirstValue);
			auto firstCompareQuery = GenerateFilterQuery(name, std::move(firstCompare));

			if (closingBracket == ')') {
				correspondingComparator = "<";
			}
			else {
				correspondingComparator = "<=";
			}
			auto secondCompare = createComparison(correspondingComparator, vSecondValue);
			auto secondCompareQuery = GenerateFilterQuery(name, std::move(secondCompare));

			return connectWithAND({ firstCompareQuery,secondCompareQuery });
		}
		else {
			throw std::invalid_argument("Query for interval incorrect. The datatype of the selected field holds no numerical value.");
		}
	}
	else {
		throw std::invalid_argument("Query for interval incorrect. The interval has to follow the english notation, e.g.: (2,3.5].");
	}
}


BsonViewOrValue AdvancedQueryBuilder::createComparisionEqualToAnyOf(const std::list<ot::Variable>& values)
{
	const std::string mongoComparator = "$in";
	return GenerateFilterQuery(mongoComparator, values);
}


BsonViewOrValue AdvancedQueryBuilder::createComparisionEqualNoneOf(const std::list<ot::Variable>& values)
{
	const std::string mongoComparator = "$nin";
	return GenerateFilterQuery(mongoComparator, values);
}

