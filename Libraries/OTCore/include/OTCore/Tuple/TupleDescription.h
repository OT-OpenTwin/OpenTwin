// @otlicense

#pragma once
#include <list>
#include <string>
#include <vector>
#include "OTCore/Serializable.h"
#include "OTCore/CoreAPIExport.h"
#include "OTCore/Tuple/TupleInstance.h"

#include <map>

class OT_CORE_API_EXPORT TupleDescription 
{
public:
	TupleDescription() = default;
	TupleDescription& operator=(const TupleDescription& _other) = default;
	TupleDescription& operator=(TupleDescription&& _other) noexcept = default;
	TupleDescription(const TupleDescription& _other) = default;
	TupleDescription(TupleDescription&& _other) noexcept = default;
	virtual ~TupleDescription() {};
			
	virtual std::string getName() const { return ""; };

	std::vector<std::string> getTupleElementNames(const std::string& _formatName) const;
	std::vector<std::string> getUnitCombinations(const std::string& _formatName) const;
	const std::vector<std::string>& getAllTupleFormatNames() const { return m_tupleFormatNames; }
		
	std::string createCombinedUnitsString(const std::vector<std::string>& _units) const;
	static std::vector<std::string> separateCombinedUnitString(const std::string& _combinedUnitString);
private:

protected:
	std::vector<std::string> m_tupleFormatNames; // All possible format names.
	std::map<std::string,std::vector<std::string>> m_tupleElementNamesByFormatName; // element names of the current format.
	std::map<std::string, std::vector<std::string>>  m_unitCombinationsByFormatName;
	

	virtual void initialiseTupleSpecifics() {};


};
