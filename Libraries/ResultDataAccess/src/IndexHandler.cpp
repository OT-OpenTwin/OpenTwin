#include "IndexHandler.h"
#include "IndexHandler.h"

IndexHandler::IndexHandler(const std::string& _collectionName, ResultImportLogger& _logger)
	: m_dataStorageAccess(_collectionName), m_logger(_logger)
{}

void IndexHandler::createDefaultIndexes()
{
	if(!checkIfDefaultIndexesAreSet())
	{
		getLogger().log("Creating default indexes for result collection.", ResultImportLoggerVerbosity::DEBUG);
	
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
	getLogger().log("Dropping all indexes except for _id index in result collection.", ResultImportLoggerVerbosity::DEBUG);
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
			const std::string& defaultIndex = getDefaultIndexes()[i];
			if (index.find(defaultIndex) != index.end()) {
				m_defaultIndexesSet[i] = true;
				break;
			}
		}
	}

	return std::all_of(m_defaultIndexesSet.begin(), m_defaultIndexesSet.end(), [](bool v) { return v; });
}

