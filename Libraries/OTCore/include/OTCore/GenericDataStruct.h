#pragma once
#include "OTCore/Serializable.h"
#pragma warning(disable:4251)

namespace ot
{
	
	class __declspec(dllexport) GenericDataStruct : public Serializable
	{
	public:
		GenericDataStruct(const std::string& _typeName = "", uint32_t _numberOfEntries = 0) :m_numberOfEntries(_numberOfEntries), m_typeName(_typeName) {}
		virtual ~GenericDataStruct() 
		{
		};

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;
		const uint32_t getNumberOfEntries() const { return m_numberOfEntries; }
		
		std::string	getTypeIdentifyer() { return m_typeName; }

	protected:
		uint32_t m_numberOfEntries;
		std::string m_typeName;
	};

	using GenericDataStructList = std::list<ot::GenericDataStruct*>;
}
