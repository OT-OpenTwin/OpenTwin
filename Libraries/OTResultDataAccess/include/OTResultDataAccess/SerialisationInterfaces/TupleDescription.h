#pragma once
#include "OTResultDataAccess/ResultDataAccessAPIExport.h"
#include <list>
#include <string>
#include <vector>

class OT_RESULTDATAACCESS_API_EXPORT TupleDescription
{
public:
	TupleDescription() = default;
	TupleDescription& operator=(const TupleDescription& _other) = default;
	TupleDescription& operator=(TupleDescription&& _other) noexcept = default;
	TupleDescription(const TupleDescription& _other) = default;
	TupleDescription(TupleDescription&& _other) noexcept = default;
	virtual ~TupleDescription() {};
	
	std::string getIDFormat() const { return m_formatID; }
	std::string getIDTuple() const { return m_tupleID; }

	std::string getFormatName() const { return m_formatName; }
	std::string getName() const { return m_name; }

	void setDataTypes(const std::vector<std::string>& _dataTypes) { m_dataTypes = _dataTypes; }
	void setUnits(const std::vector<std::string>& _units) { m_tupleUnits = _units; }

private:

	std::vector<std::string> m_dataTypes;
	std::vector<std::string> m_tupleUnits;

protected:
	std::vector<std::string> m_tupleElementNames;

	std::string m_tupleID = "";
	std::string m_name = "";

	std::string m_formatID = "";
	std::string m_formatName = "";
};
