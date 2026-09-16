#include "OTServiceFoundation/TransactionData/TransactionEntryBuilder.h"

#include "OTServiceFoundation/TransactionData/TransactionEntry.h"

void ot::TransactionEntryBuilder::initiate(const std::string& _collectionName, const std::string& _userName)
{
	if (m_logger == nullptr)
	{
		m_logger = new ot::TransactionCollectionAccess(_collectionName);
		m_userName = _userName;
	}

	m_from.clear();
	m_to.clear();
}

void ot::TransactionEntryBuilder::addFromEntity(ot::EntityIdentifier _entityID)
{
	m_from.push_back(_entityID);
}

void ot::TransactionEntryBuilder::addToEntity(ot::EntityIdentifier _entityID)
{
	m_to.push_back(_entityID);
}

void ot::TransactionEntryBuilder::log()
{
	assert(m_logger != nullptr);

	if (m_from.size() > 0 && m_to.size() > 0)
	{
		ot::TransactionEntry entry(m_userName, m_type, m_description, m_from, m_to);

		m_logger->log(entry);
	}
}
