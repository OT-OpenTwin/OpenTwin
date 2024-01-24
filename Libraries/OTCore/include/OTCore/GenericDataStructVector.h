#pragma once
#include <stdint.h>

#include "GenericDataStruct.h"
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport) GenericDataStructVector : public GenericDataStruct
	{

	public:
		GenericDataStructVector(uint32_t numberOfEntries);
		GenericDataStructVector();

		ot::Variable getValue(uint32_t index) { return _values[index]; };
		void setValue(uint32_t index, ot::Variable& value) { _values[index] = value; };
		void setValue(uint32_t index, ot::Variable&& value) { _values[index] = std::move(value); };

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		static std::string getClassName() { return "GenericDataStructVector"; }
	private:
		std::vector<ot::Variable> _values;

		void AllocateValueMemory();
	};
}