#include "OTServiceFoundation/TransactionData/TransactionLogger.h"
#include "OTDataStorage/DataStorageAPI.h"
#include "OTSystem/DateTime.h"

TransactionLogger::TransactionLogger(const std::string& _collectionName)
	:m_dataLakeAPI(_collectionName,".transactions")
{
	
}

void TransactionLogger::log(const std::string& _userName, TransactionType _transactionType, const std::string& transactionSsettings, ot::UIDList _from, ot::UIDList _to)
{
	BsonViewOrValue entry = create(_userName, _transactionType, transactionSsettings, _from, _to);
	m_dataLakeAPI.insertDocumentToDataLakePartition(entry, false, false);
	m_dataLakeAPI.flushQueuedData();
}

void TransactionLogger::searchEntry(TransactionType _transactionType, ot::UID _vertex)
{
	bsoncxx::builder::basic::document builder;
	builder.append(bsoncxx::builder::basic::kvp("transactionType", static_cast<int>(_transactionType)));
	
	
	builder.append(
		bsoncxx::builder::basic::kvp(
		"$or", bsoncxx::builder::basic::make_array(
			bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("from", static_cast<int64_t>(_vertex))),
			bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("to", static_cast<int64_t>(_vertex)))
		)
		)
	);
	mongocxx::options::find options;
	DataStorageAPI::DataStorageResponse response = m_dataLakeAPI.searchInDataLakePartition(builder.extract(),options);
	if (response.getSuccess())
	{
		std::string temp = response.getResult();
		ot::JsonDocument doc;
		doc.fromJson(temp);
		auto& entries = doc["Documents"];
	}
}

BsonViewOrValue TransactionLogger::create(const std::string& _userName, TransactionType _transactionType, const std::string& transactionSsettings, ot::UIDList _from, ot::UIDList _to)
{
	auto now = std::chrono::system_clock::now();

	// b_date has a constructor from time_point
	bsoncxx::types::b_date ts{ now };
	bsoncxx::builder::basic::document builder;
	builder.append(bsoncxx::builder::basic::kvp("userName", _userName));
	builder.append(bsoncxx::builder::basic::kvp("Date", ts));
	builder.append(bsoncxx::builder::basic::kvp("transactionType", static_cast<int>(_transactionType)));
	builder.append(bsoncxx::builder::basic::kvp("transactionSettings", transactionSsettings));
	builder.append(bsoncxx::builder::basic::kvp("from", [&_from](bsoncxx::builder::basic::sub_array subarr) {
		for (auto uid : _from) {
			subarr.append(static_cast<int64_t>(uid)) ;
		}
		}));
	builder.append(bsoncxx::builder::basic::kvp("to", [&_to](bsoncxx::builder::basic::sub_array subarr) {
		for (auto uid : _to) {
			subarr.append(static_cast<int64_t>(uid));
		}
		}));
	return builder.extract();
}