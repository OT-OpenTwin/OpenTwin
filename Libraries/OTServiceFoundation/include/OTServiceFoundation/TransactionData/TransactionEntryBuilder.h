#include "OTServiceFoundation/TransactionData/TransactionCollectionAccess.h"
#include "OTServiceFoundation/FoundationAPIExport.h"
namespace ot
{
	class OT_SERVICEFOUNDATION_API_EXPORT TransactionEntryBuilder
	{
	public:
		static TransactionEntryBuilder& INSTANCE()
		{
			static TransactionEntryBuilder INSTANCE;
			return INSTANCE;
		}
		void initiate(const std::string& _collectionName, const std::string& _userName);

		void addFromEntity(ot::EntityIdentifier _entityID);
		void addToEntity(ot::EntityIdentifier _entityID);
		void setTransactionType(ot::TransactionType _type) { m_type = _type; }
		void setTransactionDescription(const std::string& _description) { m_description = _description; }

		void log();

	private:
		TransactionEntryBuilder() = default;

		std::list<ot::EntityIdentifier> m_from;
		std::list<ot::EntityIdentifier> m_to;
		ot::TransactionType m_type = ot::TransactionType::UNKNOWN;
		ot::TransactionCollectionAccess* m_logger = nullptr;
		std::string m_userName;
		std::string m_description;
	};
};