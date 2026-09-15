#include "TransactionLoggerInstance.h"
#include "Application.h"

void TransactionLoggerInstance::initiate()
{
	if (m_logger != nullptr)
	{
		m_logger = new TransactionLogger(Application::instance()->getCollectionName());
		m_userName = Application::instance()->getLogInUserName();
	}
	m_from.clear();
	m_to.clear();
}

void TransactionLoggerInstance::addFromEntity(ot::UID _entityID)
{
	m_from.push_back(_entityID);
}

void TransactionLoggerInstance::addToEntity(ot::UID _entityID)
{
	m_to.push_back(_entityID);
}

void TransactionLoggerInstance::log()
{
	if (m_from.size() > 0 && m_to.size() > 0)
	{
		m_logger->log(m_userName, m_type, "", m_from, m_to);
	}
}

TransactionLoggerInstance::TransactionLoggerInstance()
{
}
