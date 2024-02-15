#pragma once
#include "OTCore/GenericDataStruct.h"
#include "OTCore/Variable.h"

namespace ot
{
	class __declspec(dllexport) GenericDataStructSingle : public GenericDataStruct
	{
	
	public:
		GenericDataStructSingle();
		GenericDataStructSingle(const GenericDataStructSingle& other);
		GenericDataStructSingle(GenericDataStructSingle&& other);
		GenericDataStructSingle& operator=(const GenericDataStructSingle& other);
		GenericDataStructSingle& operator=(GenericDataStructSingle&& other);
		~GenericDataStructSingle();

		void setValue(const ot::Variable& value);
		void setValue(ot::Variable&& value);

		const ot::Variable& getValue() const;

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		static std::string getClassName() { return "GenericDataStructSingle"; }
	
	private:
		ot::Variable _value;
	};

}