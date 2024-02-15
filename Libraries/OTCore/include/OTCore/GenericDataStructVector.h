#pragma once
#include <stdint.h>

#include "GenericDataStruct.h"
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport) GenericDataStructVector : public GenericDataStruct
	{

	public:
		GenericDataStructVector();
		GenericDataStructVector(const GenericDataStructVector& other);
		GenericDataStructVector(GenericDataStructVector&& other);
		
		GenericDataStructVector(const std::vector<ot::Variable>& values);
		GenericDataStructVector(std::vector<ot::Variable>&& values);
		GenericDataStructVector(uint32_t numberOfEntries);

		GenericDataStructVector& operator=(const GenericDataStructVector& other);
		GenericDataStructVector& operator=(GenericDataStructVector&& other);

		ot::Variable getValue(uint32_t index) { return _values[index]; };
		const std::vector<ot::Variable>& getValues() const { return _values; }
		
		void setValue(uint32_t index, const ot::Variable& value);
		void setValue(uint32_t index, ot::Variable&& value);
		void setValues(const std::vector<ot::Variable>& values);
		void setValues(std::vector<ot::Variable>&& values);

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		static std::string getClassName() { return "GenericDataStructVector"; }
	private:
		std::vector<ot::Variable> _values;

		void AllocateValueMemory();
	};
}