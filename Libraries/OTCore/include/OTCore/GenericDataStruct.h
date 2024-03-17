#pragma once
#include "OTCore/Serializable.h"
#pragma warning(disable:4251)

namespace ot
{
	
	class __declspec(dllexport) GenericDataStruct : public Serializable
	{
	public:
		GenericDataStruct(const std::string& typeName = "", uint32_t numberOfEntries = 0) :_numberOfEntries(numberOfEntries), _typeName(typeName) {}
		virtual ~GenericDataStruct() 
		{
		};

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;
		const uint32_t getNumberOfEntries() const { return _numberOfEntries; }
		
		std::string	getTypeIdentifyer() { return _typeName; }

	protected:
		uint32_t _numberOfEntries;
		std::string _typeName;
	};

	using GenericDataStructList = std::list<ot::GenericDataStruct*>;
}
