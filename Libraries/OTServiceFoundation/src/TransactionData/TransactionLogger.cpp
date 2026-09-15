#include "OTServiceFoundation/TransactionData/TransactionLogger.h"
#include "OTDataStorage/DataStorageAPI.h"
#include "OTSystem/DateTime.h"

TransactionLogger::TransactionLogger(const std::string& _collectionName)
	:m_dataLakeAPI(_collectionName,".transactions")
{
	
}

void TransactionLogger::log(const std::string& _userName, TransactionType _transactionType, const std::string& transactionSettings, ot::UIDList _from, ot::UIDList _to)
{
	BsonValue entry = create(_userName, _transactionType, transactionSettings, _from, _to);
	BsonViewOrValue view = entry.view();
	m_dataLakeAPI.insertDocumentToDataLakePartition(view, false, false);
	m_dataLakeAPI.flushQueuedData();
}

std::string TransactionLogger::searchEntry(TransactionType _transactionType, ot::UID _vertex)
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
		return response.getResult();		
	}
	else
	{
		return "";
	}
}

BsonValue TransactionLogger::create(const std::string& _userName, TransactionType _transactionType, const std::string& transactionSettings, ot::UIDList _from, ot::UIDList _to)
{
	auto now = std::chrono::system_clock::now();

	// b_date has a constructor from time_point
	bsoncxx::types::b_date ts{ now };
	
	bsoncxx::builder::basic::document builder;
	builder.append(bsoncxx::builder::basic::kvp("userName", _userName));
	builder.append(bsoncxx::builder::basic::kvp("Date", ts));
	builder.append(bsoncxx::builder::basic::kvp("transactionType", static_cast<int>(_transactionType)));
	builder.append(bsoncxx::builder::basic::kvp("transactionSettings", transactionSettings));

	bsoncxx::builder::basic::array from, to;
	for (auto uid : _from) 
	{
		from.append(static_cast<int64_t>(uid));
	}
	builder.append(bsoncxx::builder::basic::kvp("from", from));
	
	for (auto uid : _to)
	{
		to.append(static_cast<int64_t>(uid));
	}
	builder.append(bsoncxx::builder::basic::kvp("to", to));

	return builder.extract();
}