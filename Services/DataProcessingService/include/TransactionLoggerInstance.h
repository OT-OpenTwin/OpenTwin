#include "OTServiceFoundation/TransactionData/TransactionLogger.h"

class TransactionLoggerInstance
{
public:
	static TransactionLoggerInstance& INSTANCE()
	{
		static TransactionLoggerInstance INSTANCE;
		return INSTANCE;
	}
	void initiate();
	void addFromEntity(ot::UID _entityID);
	void addToEntity(ot::UID _entityID);
	void setTransactionType(TransactionType _type) { m_type = _type; }
	void setTransactionDescription(const std::string& _description) { m_description = _description; }
	void log();

private:
	TransactionLoggerInstance();
	ot::UIDList m_from;
	ot::UIDList m_to;
	TransactionType m_type;
	TransactionLogger* m_logger = nullptr;
	std::string m_userName;
	std::string m_description;
};
