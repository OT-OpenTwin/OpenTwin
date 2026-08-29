// @otlicense

// OpenTwin header
#include "OTCore/BasicQueue/BasicQueueObject.h"

ot::BasicQueueObject::BasicQueueObject(InsertOrder _insertOrder)
	: m_insertOrder(_insertOrder), m_isUnique(false), m_customKey(""),
	m_uniqueInsertType(UniqueQueueInsertType::ReplaceUniqueBack)
{

}

ot::BasicQueueObject::~BasicQueueObject()
{

}

ot::BasicQueueObject* ot::BasicQueueObject::makeQueueUnique(UniqueQueueInsertType _uniqueInsertType)
{
	OTAssert(!m_isUnique, "BasicQueueObject::makeQueueUnique: Object is already marked as unique.");
	m_uniqueInsertType = _uniqueInsertType;
	m_customKey.clear();
	m_isUnique = true;
	return this;
}

ot::BasicQueueObject* ot::BasicQueueObject::makeQueueUnique(const std::string& _key, UniqueQueueInsertType _uniqueInsertType)
{
	OTAssert(!m_isUnique, "BasicQueueObject::makeQueueUnique: Object is already marked as unique.");
	m_uniqueInsertType = _uniqueInsertType;
	m_customKey = _key;
	m_isUnique = true;
	return this;
}
