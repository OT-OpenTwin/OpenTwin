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
	QuantityContainer(int64_t seriesIndex, std::list<std::string>& parameterAbbreviations, std::list<ot::Variable>&& parameterValues, int64_t quantityIndex);
	QuantityContainer(QuantityContainer&& other) noexcept;
	QuantityContainer(const QuantityContainer& other) = delete;
	QuantityContainer& operator=(const QuantityContainer& other) = delete;
	QuantityContainer& operator=(QuantityContainer&& other);
	~QuantityContainer();

	void AddValue(const ot::Variable& value);
	int64_t GetValueArraySize() const { return _values.size(); };
	bsoncxx::builder::basic::document& getMongoDocument();
	static const std::string getFieldName() { return "Values"; }

private:
	bsoncxx::builder::basic::document _mongoDocument;
	std::list<ot::Variable> _values;
};
