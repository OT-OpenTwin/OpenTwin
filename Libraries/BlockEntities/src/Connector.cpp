#include "Connector.h"
#include <bsoncxx/builder/basic/kvp.hpp>

ot::Connector::Connector(ConnectorType type, const std::string& connectorName, const std::string& connectorTitle)
	:_connectorType(type), _connectorName(connectorName), _connectorTitle(connectorTitle)
{
}

ot::Connector::Connector()
	:_connectorType(UNKNOWN), _connectorName("")
{
}

bsoncxx::builder::basic::document ot::Connector::SerializeBSON() const
{
	bsoncxx::builder::basic::document newSubDocument;
	newSubDocument.append(bsoncxx::builder::basic::kvp("connectorName", _connectorName));
	newSubDocument.append(bsoncxx::builder::basic::kvp("connectorType", _connectorType));
	newSubDocument.append(bsoncxx::builder::basic::kvp("connectorTitle", _connectorTitle));
	return newSubDocument;
}

void ot::Connector::DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage)
{
	_connectorName = std::string(storage.view()["connectorName"].get_utf8().value.data());
	_connectorType = static_cast<ConnectorType>(storage.view()["connectorType"].get_int32().value);
	_connectorTitle = std::string(storage.view()["connectorTitle"].get_utf8().value.data());
}
