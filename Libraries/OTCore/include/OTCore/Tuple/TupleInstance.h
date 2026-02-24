#pragma once
#include <string>
#include <list>
#include <string>
#include <vector>
#include "OTCore/Serializable.h"
#include "OTCore/CoreAPIExport.h"

class OT_CORE_API_EXPORT TupleInstance : public ot::Serializable
{
public:
	TupleInstance() = default;
	TupleInstance(const std::string& _tupleTypeName);
	TupleInstance(const TupleInstance& _other) = default;
	TupleInstance(TupleInstance&& _other) noexcept = default;
	TupleInstance& operator=(const TupleInstance& _other) = default;
	TupleInstance& operator=(TupleInstance&& _other) noexcept = default;
	~TupleInstance() = default;
	bool operator==(const TupleInstance& _other) const;

	//´getters
	const std::string& getTupleTypeName() const { return m_tupleTypeName; }
	const std::string& getTupleFormatName() const { return m_tupleFormatName; }
	const std::vector<std::string>& getTupleUnits() const {	return m_tupleUnits;}
	const std::vector<std::string>& getTupleElementDataTypes() const { return m_tupleElementDataTypes; }
	//setters
	void setTupleTypeName(const std::string& _tupleTypeName);
	void setTupleFormatName(const std::string& _formatName);
	void setTupleUnits(const std::vector<std::string>& _units);
	void setTupleElementDataTypes(const std::vector<std::string>& _dataTypes);

	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
	void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;

	bool isSingle() const;

private:
	std::string m_tupleTypeName = "";
	std::string m_tupleFormatName = "";
	std::vector<std::string> m_tupleUnits;
	std::vector<std::string> m_tupleElementDataTypes;
};
