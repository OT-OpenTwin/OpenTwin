#pragma once
#include "BlockConnection.h"


ot::BlockConnection::BlockConnection(const std::string& connectorOrigin, const std::string& connectorDestination, ot::UID idDestination)
	:  _connectorOrigin(connectorOrigin), _connectorDestination(connectorDestination), _idDestination(idDestination)
{
}

ot::BlockConnection::BlockConnection()
	:_connectorOrigin(""), _connectorDestination(""), _idDestination(0)
{
}

std::string ot::BlockConnection::getConnectorOrigin() const
{
	return _connectorOrigin;
}

std::string ot::BlockConnection::getConnectorDestination() const
{
	return _connectorDestination;
}

ot::UID ot::BlockConnection::getIDDestination() const
{
	return _idDestination;
}

bsoncxx::builder::basic::document ot::BlockConnection::SerializeBSON() const
{
	bsoncxx::builder::basic::document newSubdocument;
	newSubdocument.append(bsoncxx::builder::basic::kvp("connectorOrigin", _connectorOrigin));
	newSubdocument.append(bsoncxx::builder::basic::kvp("connectorDestination", _connectorDestination));
	newSubdocument.append(bsoncxx::builder::basic::kvp("idDestination", static_cast<int64_t>(_idDestination)));
	return newSubdocument;
}

void ot::BlockConnection::DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage)
{
	_connectorOrigin = storage.view()["connectorOrigin"].get_utf8().value.to_string();
	_connectorDestination = storage.view()["connectorDestination"].get_utf8().value.to_string();
	_idDestination = static_cast<uint64_t>(_idDestination = storage.view()["idDestination"].get_int64().value);
}
