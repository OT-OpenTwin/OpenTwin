#pragma once
//@otlicense
#include <bsoncxx/document/view_or_value.hpp>
#include <string>
#include "OTServiceFoundation/TransactionData/TransactionType.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"

namespace ot
{
	class OT_SERVICEFOUNDATION_API_EXPORT TransactionEntry : ot::Serializable
	{
	public:
		TransactionEntry(const std::string& _userName, TransactionType _transactionType, const std::string& _transactionSettings, const std::list<ot::EntityIdentifier>& _from, const std::list<ot::EntityIdentifier>& _to)
			:m_userName(_userName), m_transactionType(_transactionType), m_transactionSettings(_transactionSettings), m_from(_from), m_to(_to)
		{
		}
		TransactionEntry() = default;

		// Inherited via Serializable
		void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		const std::string& getUserName() { return m_userName;}
		TransactionType getTransactionType() { return m_transactionType; }
		const std::string& getTransactionSettings() { return m_transactionSettings; }
		const std::list<ot::EntityIdentifier>& getEntityIdentifierFrom() const { return m_from; }
		const std::list<ot::EntityIdentifier>& getEntityIdentifierTo() const { return m_to; }

	private:
		std::string m_userName;
		TransactionType m_transactionType = TransactionType::UNKNOWN;
		std::string m_transactionSettings;
		std::list<ot::EntityIdentifier> m_from;
		std::list<ot::EntityIdentifier> m_to;

	};
};