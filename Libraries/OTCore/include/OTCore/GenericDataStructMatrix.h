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
		GenericDataStructMatrix(const GenericDataStructMatrix& _other);
		GenericDataStructMatrix(GenericDataStructMatrix&& _other)noexcept;
		GenericDataStructMatrix& operator=(const GenericDataStructMatrix& _other);
		GenericDataStructMatrix& operator=(GenericDataStructMatrix&& _other) noexcept;


		GenericDataStructMatrix(uint32_t _numberofColumns, uint32_t _numberofRows);
		GenericDataStructMatrix(uint32_t _numberofColumns, uint32_t _numberofRows, ot::Variable _defaultValue);
		
		void setValue(uint32_t _columnIndex, uint32_t _rowIndex, ot::Variable&& _value);
		void setValue(uint32_t _columnIndex, uint32_t _rowIndex, const ot::Variable& _value);
		void setValues(const ot::Variable* _values, uint32_t _size);
		/// <summary>
		/// column-major access
		/// </summary>
		const ot::Variable& getValue(uint32_t _columnIndex, uint32_t _rowIndex)const;
		const ot::Variable* getValues()const;
		const uint32_t getNumberOfColumns() const { return m_numberOfColumns; }
		const uint32_t getNumberOfRows() const { return m_numberOfRows; }

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		static std::string getClassName() { return "GenericDataStructMatrix"; }

	private:
		uint32_t m_numberOfColumns = 0;
		uint32_t m_numberOfRows = 0;
		
		std::vector<ot::Variable> m_values;
		void allocateValueMemory();
		void allocateValueMemory( const ot::Variable& _defaultValue);
		void allocateValueMemory( ot::Variable&& _defaultValue);
		
		inline uint32_t getIndex(const uint32_t& _columnIndex, const uint32_t& _rowIndex) const { return m_numberOfColumns * _rowIndex + _columnIndex; }
	};
}
