#include "OTServiceFoundation/TransactionData/TransactionCollectionAccess.h"
#include "OTDataStorage/DataStorageAPI.h"
#include "OTSystem/DateTime.h"

ot::TransactionCollectionAccess::TransactionCollectionAccess(const std::string& _collectionName)
	:m_dataLakeAPI(_collectionName, ".transactions")
{
}

void ot::TransactionCollectionAccess::log(TransactionEntry& _transactionEntry)
{
	BsonValue entry = toBSON(_transactionEntry);
	BsonViewOrValue view = entry.view();
	m_dataLakeAPI.insertDocumentToDataLakePartition(view, false, false);
	m_dataLakeAPI.flushQueuedData();
}

std::list<ot::TransactionEntry> ot::TransactionCollectionAccess::searchEntry(TransactionType _transactionType, ot::EntityIdentifier _startVertex)
{

	// Query targets a tuple of id and version.
	// A query for a specific field of the tuple, e.g. the id could be generated on this base (here query for both id and version):
	//	from: {
	//		$elemMatch: {
	//			$and: [
	//				{ $arrayElemAt: ["$$this", 0] : "_startVertex.m_id" },
	//				{ $arrayElemAt: ["$$this", 1] : "_startVertex.m_version" }
	//			]
	//		}
	//	}
	// Tupel [id, version] als BSON-Array
	auto tupleArray = bsoncxx::builder::basic::make_array(
		static_cast<int64_t>(_startVertex.m_id),
		static_cast<int64_t>(_startVertex.m_version)
	);

	// $elemMatch: { $eq: [id, version] }
	auto elemMatchCond = bsoncxx::builder::basic::make_document(
		bsoncxx::builder::basic::kvp("$eq", tupleArray)
	);

	// from: { $elemMatch: { $eq: [id, version] } }
	auto fromCond = bsoncxx::builder::basic::make_document(
		bsoncxx::builder::basic::kvp("from", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$elemMatch", elemMatchCond.view())))
	);

	// to: { $elemMatch: { $eq: [id, version] } }
	auto toCond = bsoncxx::builder::basic::make_document(
		bsoncxx::builder::basic::kvp("to", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$elemMatch", elemMatchCond.view())))
	);

	// $or: [ fromCond, toCond ]
	bsoncxx::builder::basic::document builder;
	builder.append(
		bsoncxx::builder::basic::kvp("$or", bsoncxx::builder::basic::make_array(fromCond, toCond))
	);

	mongocxx::options::find options;
	DataStorageAPI::DataStorageResponse response = m_dataLakeAPI.searchInDataLakePartition(builder.extract(), options);
	std::list<ot::TransactionEntry> allMatchingEntries;

	if (response.getSuccess())
	{
		const std::string& result = response.getResult();
		ot::JsonDocument resultDoc;
		resultDoc.fromJson(result);
		const auto& entries = ot::json::getArray(resultDoc, "Documents");
		for (auto& entry : entries)
		{
			ot::TransactionEntry transactionEntry;
			transactionEntry.setFromJsonObject(entry.GetObject());
			allMatchingEntries.push_back(transactionEntry);
		}
	}
	
	return allMatchingEntries;
}

bsoncxx::document::value ot::TransactionCollectionAccess::toBSON(ot::TransactionEntry _transactionEntry)
{
	// !! Attention !! The field keys need to have the same names as in TransactionEntry::addToJsonObject. The returned bson documents are interpreted as json and turned into TransactionEntry objects

	auto now = std::chrono::system_clock::now();

	// b_date has a constructor from time_point
	bsoncxx::types::b_date ts{ now };
	
	bsoncxx::builder::basic::document builder;
	builder.append(bsoncxx::builder::basic::kvp("userName", _transactionEntry.getUserName()));
	builder.append(bsoncxx::builder::basic::kvp("Date", ts));
	builder.append(bsoncxx::builder::basic::kvp("transactionType", Transaction::toInt(_transactionEntry.getTransactionType())));
	builder.append(bsoncxx::builder::basic::kvp("transactionSettings", _transactionEntry.getTransactionSettings()));

	bsoncxx::builder::basic::array from, to;
	for (auto identifier : _transactionEntry.getEntityIdentifierFrom())
	{
		bsoncxx::builder::basic::array identifierArray;
		identifierArray.append(static_cast<int64_t>(identifier.m_id));
		identifierArray.append(static_cast<int64_t>(identifier.m_version));
		from.append(identifierArray);
	}
	builder.append(bsoncxx::builder::basic::kvp("from", from));

	for (auto identifier : _transactionEntry.getEntityIdentifierTo())
	{
		bsoncxx::builder::basic::array identifierArray;
		identifierArray.append(static_cast<int64_t>(identifier.m_id));
		identifierArray.append(static_cast<int64_t>(identifier.m_version));
		to.append(identifierArray);
	}
	builder.append(bsoncxx::builder::basic::kvp("to", to));

	return builder.extract();
}

