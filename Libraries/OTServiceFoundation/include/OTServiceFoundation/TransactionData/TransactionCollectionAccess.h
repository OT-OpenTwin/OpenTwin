#pragma once
//@otlicense
#include "OTDataStorage/DataLakeAPI.h"
#include "OTServiceFoundation/TransactionData/TransactionType.h"
#include "OTCore/UIDNamePair.h"
#include "OTServiceFoundation/FoundationAPIExport.h"
#include "OTServiceFoundation/TransactionData/TransactionEntry.h"
namespace ot
{
	class OT_SERVICEFOUNDATION_API_EXPORT TransactionCollectionAccess
	{
	public:
		enum class EdgeDirection
		{
			TowardsStartVertex,
			FromStartVertex,
			BothDirections
		};

		TransactionCollectionAccess(const std::string& _collectionName);
		void log(TransactionEntry& _transactionEntry);
		std::list<ot::TransactionEntry> searchEntry(TransactionType _transactionType, ot::EntityIdentifier _startVertex, EdgeDirection _edgeDirection = EdgeDirection::BothDirections);

	private:
		DataStorageAPI::DataLakeAPI m_dataLakeAPI;

		bsoncxx::document::value toBSON(ot::TransactionEntry _transactionEntry);
	};
}