// @otlicense

// OpenTwin header
#include "OTCore/BasicQueue/BasicQueueObject.h"

ot::BasicQueueObject::BasicQueueObject(InsertOrder _insertOrder)
	: m_insertOrder(_insertOrder), m_isUnique(false), m_customKey(""),
	m_uniqueInsertBehavior(UniqueInsertBahavior::RemoveExisting)
{

}

ot::BasicQueueObject::~BasicQueueObject()
{

}

ot::BasicQueueObject* ot::BasicQueueObject::makeQueueUnique(UniqueInsertBahavior _uniqueInsertBehavior)
{
	OTAssert(!m_isUnique, "BasicQueueObject::makeQueueUnique: Object is already marked as unique.");
	m_uniqueInsertBehavior = _uniqueInsertBehavior;
	m_customKey.clear();
	m_isUnique = true;
	return this;
}

ot::BasicQueueObject* ot::BasicQueueObject::makeQueueUnique(const std::string& _key, UniqueInsertBahavior _uniqueInsertBehavior)
{
	OTAssert(!m_isUnique, "BasicQueueObject::makeQueueUnique: Object is already marked as unique.");
	m_uniqueInsertBehavior = _uniqueInsertBehavior;
	m_customKey = _key;
	m_isUnique = true;
	return this;
}
