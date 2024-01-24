#pragma once

#include <stdint.h>
#include <vector>

#include "OTCore/Variable.h"
#include "GenericDataStruct.h"

namespace ot
{
	class __declspec(dllexport) GenericDataStructMatrix : public GenericDataStruct
	{
	public:
		GenericDataStructMatrix(uint32_t numberofColumns, uint32_t numberofRows);
		GenericDataStructMatrix();
		~GenericDataStructMatrix();

		void setValue(uint32_t columnIndex, uint32_t rowIndex, ot::Variable&& value);
		void setValue(uint32_t columnIndex, uint32_t rowIndex, const ot::Variable& value);
		const ot::Variable& getValue(uint32_t columnIndex, uint32_t rowIndex)const;
		const uint32_t getNumberOfColumns() const { return _numberOfColumns; }
		const uint32_t getNumberOfRows() const { return _numberOfRows; }

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		static std::string getClassName() { return "GenericDataStructMatrix"; }

	private:
		uint32_t _numberOfColumns = 0;
		uint32_t _numberOfRows = 0;
		std::vector<std::vector<ot::Variable>> _values;

		void AllocateValueMemory();
		/*ot::Variable* _values = nullptr;
		inline uint32_t getIndex(const uint32_t& columnIndex, const uint32_t& rowIndex) const { return _numberOfColumns * rowIndex + columnIndex; }*/
	};
}
