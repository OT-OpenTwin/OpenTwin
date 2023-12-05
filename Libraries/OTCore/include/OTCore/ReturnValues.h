#pragma once
#include <map>

#include "OpenTwinCore/Variable.h"
#include "OpenTwinCore/Serializable.h"

namespace ot
{
	class OT_CORE_API_EXPORT ReturnValues : public ot::Serializable
	{
	public:
		void addData(const std::string& entryName, const std::list<ot::Variable>& values);
		void addData(const std::string& entryName, std::list<ot::Variable>&& values);
		const size_t getNumberOfEntries() const { return _valuesByName.size(); }
		const std::map<std::string, std::list<ot::Variable>>& getValuesByName() const { return _valuesByName; }


		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

	private:
		std::map<std::string, std::list<ot::Variable>> _valuesByName;

	};
}