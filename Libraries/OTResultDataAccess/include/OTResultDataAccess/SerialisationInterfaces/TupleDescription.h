#pragma once
#include "OTResultDataAccess/ResultDataAccessAPIExport.h"
#include <list>
#include <string>
#include <vector>
#include "OTCore/Serializable.h"


class OT_RESULTDATAACCESS_API_EXPORT TupleDescription : public ot::Serializable
{
public:
	TupleDescription() = default;
	TupleDescription& operator=(const TupleDescription& _other) = default;
	TupleDescription& operator=(TupleDescription&& _other) noexcept = default;
	TupleDescription(const TupleDescription& _other) = default;
	TupleDescription(TupleDescription&& _other) noexcept = default;
	virtual ~TupleDescription() {};
	
	std::string getFormatName() const { return m_formatName; }
	std::string getName() const { return m_name; }

	const std::vector<std::string>& getTupleElementNames() const { return m_tupleElementNames; }
	const std::string& getDataType() const {return m_dataType;}
	const std::vector<std::string>& getUnits() const { return m_tupleUnits; }

	void setDataType(const std::string& _dataType) { m_dataType = _dataType; }
	void setUnits(const std::vector<std::string>& _units) { m_tupleUnits = _units; }
	void setName(const std::string& _name) { m_name = _name; }
	void setFormatName(const std::string& _formatName) 
	{ 
		m_formatName = _formatName; 
		initialiseTupleElementNames();
	}


	bool operator==(const TupleDescription& _other) const 
	{
		return m_name == _other.m_name &&
			m_formatName == _other.m_formatName &&
			m_tupleElementNames == _other.m_tupleElementNames &&
			m_dataType == _other.m_dataType &&
			m_tupleUnits == _other.m_tupleUnits;
	}

	// Inherited via Serializable
	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
	void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;
private:

	std::string m_dataType;
	std::vector<std::string> m_tupleUnits;

protected:
	std::vector<std::string> m_tupleElementNames;
	std::string m_name = "";
	std::string m_formatName = "";

	virtual void initialiseTupleElementNames() {};


};
