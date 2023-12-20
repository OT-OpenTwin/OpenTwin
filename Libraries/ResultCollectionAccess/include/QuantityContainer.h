#pragma once
#include <stdint.h>
#include <list>
#include <string>
#include <map>

#include "OTCore/Variable.h"


#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>


class __declspec(dllexport) QuantityContainer
{
public:
	QuantityContainer(int32_t seriesIndex, std::list<std::string>& parameterAbbreviations, std::list<ot::Variable>&& parameterValues, int32_t quantityIndex);
	QuantityContainer(QuantityContainer&& other);
	QuantityContainer(const QuantityContainer& other) = delete;
	QuantityContainer operator=(const QuantityContainer& other) = delete;
	QuantityContainer operator=(QuantityContainer&& other);
	~QuantityContainer();

	bsoncxx::builder::basic::document* GetDocument() { return &_mongoDocument; };

	void AddValue(const ot::Variable& value);
	int64_t GetValueArraySize() const { return _values.size(); };
	const bsoncxx::builder::basic::document& getMongoDocument();
	static const std::string getFieldName() { return "Values"; }

private:
	bsoncxx::builder::basic::document _mongoDocument;
	std::list<ot::Variable> _values;
};
