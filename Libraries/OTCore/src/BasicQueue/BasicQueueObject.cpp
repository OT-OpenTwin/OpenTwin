// @otlicense
// File: BasicQueueObject.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/BasicQueue/BasicQueueObject.h"

ot::BasicQueueObject::BasicQueueObject(InsertOrder _insertOrder)
	: m_insertOrder(_insertOrder), m_isUnique(false), m_customKey(""),
	m_uniqueInsertBehavior(UniqueInsertBahavior::RemoveExisting)
{

}

ot::BasicQueueObject::~BasicQueueObject()
{}

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
