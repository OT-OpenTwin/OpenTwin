#pragma once
#include "OTModelAPI/ModelServiceAPI.h"
class EntityLazyIterator
{
public:
    EntityLazyIterator(const std::list<ot::EntityInformation>& _entityInfos, uint32_t _batchSize);
	
    struct Iterator {
        using iterator_category = std::input_iterator_tag;
        using value_type = EntityBase;
        using difference_type = std::ptrdiff_t;
        using pointer = const EntityBase*;
        using reference = const EntityBase&;

        EntityLazyIterator* m_owner;
        std::size_t m_localIndex;   // position within current batch
        bool        m_done;

        Iterator(EntityLazyIterator* _owner, bool _done)
            : m_owner(_owner), m_localIndex(0), m_done(_done)
        {
        }

        const EntityBase& operator*() const {
            return *m_owner->m_buffer[m_localIndex];
        }

        Iterator& operator++() {
            ++m_localIndex;

            // Crossed batch border → refill
            if (m_localIndex >= m_owner->m_buffer.size()) 
            {
                ++m_owner->m_batchIndex;
                m_owner->clearBuffer();
                bool hasMore = m_owner->loadBatch();
                m_localIndex = 0;

                if (!hasMore || m_owner->m_buffer.empty()) 
                {
                    m_done = true;
                }
            }
            return *this;
        }

        bool operator!=(const Iterator& other) const 
        {
            return m_done != other.m_done;
        }
    };

    Iterator begin() 
    {
        m_batchIndex = 0;
        clearBuffer();
        m_buffer.reserve(m_batchSize);

        bool hasData = loadBatch();
        if (!hasData || m_buffer.empty()) 
        {
            return Iterator(this, true);   // empty source
        }
        return Iterator(this, false);
    }

    Iterator end() 
    {
        return Iterator(this, true);
    }

private:
	uint32_t m_batchSize;
	const std::vector<ot::EntityInformation> m_entityInfosForLoading;
	std::vector<EntityBase*> m_buffer;
    std::size_t m_batchIndex = 0;

    bool loadBatch();
    void clearBuffer();
};
