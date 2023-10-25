#pragma once
#include "BlockConnection.h"


ot::BlockConnection::BlockConnection(const ot::GraphicsConnectionCfg& connection)
	: _connection(connection)
{
}

const ot::GraphicsConnectionCfg& ot::BlockConnection::getConnection() const
{
	return _connection;
}

void ot::BlockConnection::setConnection(const ot::GraphicsConnectionCfg& connection)
{
	_connection = connection;
}

bsoncxx::builder::basic::document ot::BlockConnection::SerializeBSON() const
{
	bsoncxx::builder::basic::document newSubdocument;
	newSubdocument.append(bsoncxx::builder::basic::kvp("FromConnectable", _connection.originConnectable()));
	newSubdocument.append(bsoncxx::builder::basic::kvp("ToConnectable", _connection.destConnectable()));
	newSubdocument.append(bsoncxx::builder::basic::kvp("FromUID", _connection.originUid()));
	newSubdocument.append(bsoncxx::builder::basic::kvp("ToUID", _connection.destUid()));
	return newSubdocument;
}

void ot::BlockConnection::DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage)
{
	_connection.setOriginConnectable(storage.view()["FromConnectable"].get_utf8().value.to_string());
	_connection.setDestConnectable(storage.view()["ToConnectable"].get_utf8().value.to_string());
	_connection.setOriginUid(storage.view()["FromUID"].get_utf8().value.to_string());
	_connection.setDestUid(storage.view()["ToUID"].get_utf8().value.to_string());
}

