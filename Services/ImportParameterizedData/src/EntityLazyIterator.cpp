#include "EntityLazyIterator.h"
#include "Application.h"
#include "OTModelEntities/EntityAPI.h"
inline EntityLazyIterator::EntityLazyIterator(const std::list<ot::EntityInformation>& _entityInfos, uint32_t _batchSize)
	:m_entityInfosForLoading(_entityInfos.begin(), _entityInfos.end()) , m_batchSize(_batchSize)
{}

inline bool EntityLazyIterator::loadBatch()
{
    std::size_t start = m_batchIndex * m_batchSize;
    if (start >= m_entityInfosForLoading.size())
    {
        return false;
    }

    std::size_t end = std::min(start + m_batchSize,m_entityInfosForLoading.size());
    std::list<ot::EntityInformation> batchEntities;
    for (std::size_t i = start; i < end; ++i)
    {
        batchEntities.push_back(m_entityInfosForLoading[i]);
    }
    
    Application::instance()->prefetchDocumentsFromStorage(batchEntities);
    for (ot::EntityInformation& batchEntityInfo : batchEntities)
    {
        m_buffer.push_back(ot::EntityAPI::readEntityFromEntityIDandVersion(batchEntityInfo));
    }

    return true;
}

void EntityLazyIterator::clearBuffer()
{
    for (EntityBase* bufferedEntity : m_buffer)
    {
        delete bufferedEntity;
        bufferedEntity = nullptr;
    }
    m_buffer.clear();
}
