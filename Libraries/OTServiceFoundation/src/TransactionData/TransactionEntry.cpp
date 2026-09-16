#include "OTServiceFoundation/TransactionData/TransactionEntry.h"

#include <chrono>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>

void ot::TransactionEntry::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	// !! Attention !! The field keys need to have the same names as in TransactionCollectionAccess::toBSON. Queried bson documents are interpreted as json and turned into TransactionEntry objects

	_jsonObject.AddMember("userName", ot::JsonString(m_userName, _allocator), _allocator);
	_jsonObject.AddMember("transactionType", Transaction::toInt(m_transactionType), _allocator);
	_jsonObject.AddMember("transactionSettings", ot::JsonString(m_transactionSettings,_allocator), _allocator);

	ot::JsonArray from, to;
	for (auto identifier : m_from)
	{
		ot::JsonArray identifierArray;
		identifierArray.PushBack(static_cast<int64_t>(identifier.m_id), _allocator);
		identifierArray.PushBack(static_cast<int64_t>(identifier.m_version), _allocator);
		
		from.PushBack(identifierArray, _allocator);
	}
	_jsonObject.AddMember("from", from, _allocator);

	for (auto identifier : m_to)
	{
		ot::JsonArray identifierArray;
		identifierArray.PushBack(static_cast<int64_t>(identifier.m_id), _allocator);
		identifierArray.PushBack(static_cast<int64_t>(identifier.m_version), _allocator);

		to.PushBack(identifierArray, _allocator);
	}
	_jsonObject.AddMember("to", to, _allocator);
}

void ot::TransactionEntry::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	m_userName = ot::json::getString(_jsonObject, "userName");
	m_transactionSettings = ot::json::getString(_jsonObject, "transactionSettings");
	int32_t temp = ot::json::getInt(_jsonObject, "transactionType");
	m_transactionType = ot::Transaction::fromInt(temp);

	ot::ConstJsonArray from = ot::json::getArray(_jsonObject, "from");
	for (int i = 0 ; i < from.Size() ; i++)
	{
		ot::ConstJsonArray identifier = ot::json::getArray(from, i);
		ot::EntityIdentifier ident;
		ident.m_id = ot::json::getInt64(identifier, 0);
		ident.m_version = ot::json::getInt64(identifier, 1);
		m_from.push_back(ident);
	}
	
	ot::ConstJsonArray to = ot::json::getArray(_jsonObject, "to");
	for (int i = 0; i < to.Size(); i++)
	{
		ot::ConstJsonArray identifier = ot::json::getArray(to, i);
		ot::EntityIdentifier ident;
		ident.m_id = ot::json::getInt64(identifier, 0);
		ident.m_version = ot::json::getInt64(identifier, 1);
		m_to.push_back(ident);
	}
}
