// @otlicense
// File: IndexHandler.cpp
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

#include "IndexHandler.h"
#include "OTServiceFoundation/ProgressUpdater.h"

IndexHandler::IndexHandler(const std::string& _collectionName)
	: m_dataStorageAccess(_collectionName)
{}

void IndexHandler::createDefaultIndexes()
{
	if(!checkIfDefaultIndexesAreSet())
	{
		const bool continuousProgressbar = true;
	
		mongocxx::collection& collection =	m_dataStorageAccess.getCollection();

		for(size_t i = 0; i < getDefaultIndexes().size(); i++)
		{
			if (!m_defaultIndexesSet[i])
			{
				const std::string& indexName = getDefaultIndexes()[i];
				bsoncxx::builder::basic::document index{};
				index.append(bsoncxx::builder::basic::kvp(indexName, 1));
				collection.create_index(index.view());
			}
		}
	}
}

void IndexHandler::dropAllIndexes()
{
	mongocxx::collection& collection = m_dataStorageAccess.getCollection();
	collection.indexes().drop_all();
	
	// Reset the default indexes set tracking
	std::fill(m_defaultIndexesSet.begin(), m_defaultIndexesSet.end(), false);
}

bool IndexHandler::checkIfDefaultIndexesAreSet()
{
	mongocxx::collection& collection = m_dataStorageAccess.getCollection();
	auto allIndexes = collection.indexes().list();
	for (auto&& index : allIndexes)
	{
		for (size_t i = 0; i < getDefaultIndexes().size(); ++i)
		{
			
			auto&& key = index["key"].get_document();
			
			const std::string& defaultIndex = getDefaultIndexes()[i];
			if (key.view().find(defaultIndex) != key.view().end()) {
				m_defaultIndexesSet[i] = true;
				break;
			}
		}
	}

	return std::all_of(m_defaultIndexesSet.begin(), m_defaultIndexesSet.end(), [](bool v) { return v; });
}

