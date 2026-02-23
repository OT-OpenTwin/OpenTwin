// @otlicense

#pragma once
#include "OTResultDataAccess/ResultDataAccessAPIExport.h"
#include <list>
#include <string>
#include <vector>
#include "OTCore/Serializable.h"


class TupleDescription : public ot::Serializable
{
public:
	TupleDescription() = default;
	TupleDescription& operator=(const TupleDescription& _other) = default;
	TupleDescription& operator=(TupleDescription&& _other) noexcept = default;
	TupleDescription(const TupleDescription& _other) = default;
	TupleDescription(TupleDescription&& _other) noexcept = default;
	virtual ~TupleDescription() {};
	bool operator==(const TupleDescription& _other) const;
	
	std::string getFormatName() const { return m_formatName; }
		
	virtual std::string getName() const { return ""; };
	const std::vector<std::string>& getTupleElementNames() const { return m_tupleElementNames; }
	const std::vector<std::string>& getTupleFormatNames() const { return m_tupleFormatNames; }

	const std::string& getDataType() const {return m_dataType;}
	const std::vector<std::string>& getUnits() const { return m_tupleUnits; }

	void setDataType(const std::string& _dataType) { m_dataType = _dataType; }
	void setUnits(const std::vector<std::string>& _units) { m_tupleUnits = _units; }
	virtual void setName(const std::string& _name) {}
	void setFormatName(const std::string& _formatName);

	bool isSingle() const { return m_tupleElementNames.size() <= 1; }

	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
	void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;

	virtual TupleDescription* clone() { return new TupleDescription(*this); };
private:
	std::string m_dataType;
	std::vector<std::string> m_tupleUnits;

protected:
	std::vector<std::string> m_tupleElementNames;
	std::vector<std::string> m_tupleFormatNames;
	std::string m_formatName = "";

	virtual void initialiseTupleSpecifics() {};


};
