// @otlicense
// File: ParallelCollectionRAII.cpp
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
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/RAII/ParallelCollectionRAII.h"

ot::ParallelCollectionRAII::ParallelCollectionRAII(const std::string& _otherCollection, const ConstructionFlags& _constructionFlags)
	: m_state(StateFlag::IsValid | StateFlag::IsInitial), m_initial(DataBase::instance().getCollectionName()), m_other(_otherCollection)
{
	OTAssert(!m_initial.empty(), "Initial collection name is empty");
	OTAssert(!m_other.empty(), "Other collection name is empty");

	if (_constructionFlags.has(ConstructionFlag::ResetOnDestruction))
	{
		m_state.set(StateFlag::ResetOnDestroy);
	}

	if (_constructionFlags.has(ConstructionFlag::SwitchOnConstruction))
	{
		this->switchToOther();
	}
}

ot::ParallelCollectionRAII::ParallelCollectionRAII(ParallelCollectionRAII&& _other) noexcept
	: m_state(std::move(_other.m_state)), m_initial(std::move(_other.m_initial)), m_other(std::move(_other.m_other))
{
	_other.m_state.clear();
}

ot::ParallelCollectionRAII::~ParallelCollectionRAII()
{
	if (m_state.has(StateFlag::ResetOnDestroy))
	{
		this->switchToInitial();
	}
}

void ot::ParallelCollectionRAII::switchToInitial()
{
	if (m_state.has(StateFlag::IsValid) && !m_state.has(StateFlag::IsInitial))
	{
		m_state.set(StateFlag::IsInitial);

		DataBase::instance().setCollectionName(m_initial);
		DataBase::instance().removePrefetchedDocument(0);
	}
}

void ot::ParallelCollectionRAII::switchToOther()
{
	if (m_state.has(StateFlag::IsValid | StateFlag::IsInitial) && m_initial != m_other)
	{
		m_state.remove(StateFlag::IsInitial);

		DataBase::instance().setCollectionName(m_other);
		DataBase::instance().removePrefetchedDocument(0);
	}
}
