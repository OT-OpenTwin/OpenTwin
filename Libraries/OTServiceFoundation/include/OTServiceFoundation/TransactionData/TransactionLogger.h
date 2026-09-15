#pragma once
//@otlicense
#include "OTDataStorage/DataLakeAPI.h"
#include "OTServiceFoundation/TransactionData/TransactionType.h"
#include "OTCore/UIDNamePair.h"
#include "OTServiceFoundation/FoundationAPIExport.h"

class OT_SERVICEFOUNDATION_API_EXPORT TransactionLogger
{
public:
	TransactionLogger(const std::string& _collectionName);
	void log(const std::string& _userName, TransactionType _transactionType, const std::string& transactionSsettings, ot::UIDList _from, ot::UIDList _to);
	std::string searchEntry(TransactionType _transactionType, ot::UID _vertex);

private:
	DataStorageAPI::DataLakeAPI m_dataLakeAPI;

	static BsonViewOrValue create(const std::string& _userName, TransactionType _transactionType, const std::string& transactionSsettings, ot::UIDList _from, ot::UIDList _to);
	

};
