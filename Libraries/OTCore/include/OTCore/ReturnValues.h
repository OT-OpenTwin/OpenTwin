#pragma once
#include <map>

#include "OTCore/Variable.h"
#include "OTCore/GenericDataStruct.h"
#include "OTCore/Serializable.h"

namespace ot
{
	class OT_CORE_API_EXPORT ReturnValues : public ot::Serializable
	{
	public:
		ReturnValues() {}
		~ReturnValues();

		/// <summary>
		/// Deep copy of GenericDataStructs
		/// </summary>
		/// <param name="other"></param>
		ReturnValues(const ReturnValues& other);
		ReturnValues(ReturnValues&& other) noexcept;

		/// <summary>
		/// Deep copy of GenericDataStructs
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		ReturnValues& operator=(const ReturnValues& other);
		ReturnValues& operator=(ReturnValues&& other) noexcept;

		bool operator==(const ReturnValues& other) const;
		bool operator!=(const ReturnValues& other) const;

		void addData(const std::string& entryName, const GenericDataStructList& values);
		void addData(const std::string& entryName, GenericDataStructList&& values);
		const size_t getNumberOfEntries() const { return _valuesByName.size(); }
		std::map<std::string, GenericDataStructList>& getValuesByName() { return _valuesByName; }


		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	private:
		std::map<std::string, GenericDataStructList> _valuesByName;

	};
}