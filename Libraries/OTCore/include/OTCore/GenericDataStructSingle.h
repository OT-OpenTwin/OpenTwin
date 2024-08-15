#pragma once
#include "OTCore/GenericDataStruct.h"
#include "OTCore/Variable.h"

namespace ot
{
	class __declspec(dllexport) GenericDataStructSingle : public GenericDataStruct
	{
	
	public:
		GenericDataStructSingle();
		GenericDataStructSingle(const GenericDataStructSingle& _other);
		GenericDataStructSingle(GenericDataStructSingle&& _other) noexcept;
		GenericDataStructSingle& operator=(const GenericDataStructSingle& _other);
		GenericDataStructSingle& operator=(GenericDataStructSingle&& _other) noexcept;
		~GenericDataStructSingle();

		void setValue(const ot::Variable& _value);
		void setValue(ot::Variable&& _value);

		const ot::Variable& getValue() const;

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		static std::string getClassName() { return "GenericDataStructSingle"; }
	
	private:
		ot::Variable m_value;
	};

}