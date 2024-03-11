#pragma once

#include <stdint.h>
#include <vector>
#include <functional>

#include "OTCore/Variable.h"
#include "GenericDataStruct.h"

namespace ot
{
	class __declspec(dllexport) GenericDataStructMatrix : public GenericDataStruct
	{
	public:
		GenericDataStructMatrix();
		~GenericDataStructMatrix();
		GenericDataStructMatrix(const GenericDataStructMatrix& other);
		GenericDataStructMatrix(GenericDataStructMatrix&& other)noexcept;
		GenericDataStructMatrix& operator=(const GenericDataStructMatrix& other);
		GenericDataStructMatrix& operator=(GenericDataStructMatrix&& other) noexcept;


		GenericDataStructMatrix(uint32_t numberofColumns, uint32_t numberofRows);
		
		void setValue(uint32_t columnIndex, uint32_t rowIndex, ot::Variable&& value);
		void setValue(uint32_t columnIndex, uint32_t rowIndex, const ot::Variable& value);
		void setValues(const ot::Variable* values, uint32_t size);
		/// <summary>
		/// column-major access
		/// </summary>
		const ot::Variable& getValue(uint32_t columnIndex, uint32_t rowIndex)const;
		const ot::Variable* getValues()const;
		const uint32_t getNumberOfColumns() const { return _numberOfColumns; }
		const uint32_t getNumberOfRows() const { return _numberOfRows; }

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		static std::string getClassName() { return "GenericDataStructMatrix"; }

	private:
		uint32_t _numberOfColumns = 0;
		uint32_t _numberOfRows = 0;
		//void* _values = nullptr;
		std::vector<ot::Variable> _values;
		//std::string _typeName;
		/*std::function<void(int32_t index, const ot::Variable&)> _setter;
		std::function<void(int32_t index, ot::Variable&&)> _setterMove;
		std::function<ot::Variable(int32_t index)> _getter;*/

		void AllocateValueMemory(/*const std::string& typeName*/);
		
		inline uint32_t getIndex(const uint32_t& columnIndex, const uint32_t& rowIndex) const { return _numberOfColumns * rowIndex + columnIndex; }
	};
}
