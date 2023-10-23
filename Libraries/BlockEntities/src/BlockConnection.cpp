#pragma once
#include "BlockConnection.h"


ot::BlockConnection::BlockConnection(const ot::GraphicsConnectionPackage::ConnectionInfo& connection)
	: _connection(connection)
{
}

const ot::GraphicsConnectionPackage::ConnectionInfo& ot::BlockConnection::getConnection() const
{
	return _connection;
}

void ot::BlockConnection::setConnection(const ot::GraphicsConnectionPackage::ConnectionInfo& connection)
{
	_connection = connection;
}

bsoncxx::builder::basic::document ot::BlockConnection::SerializeBSON() const
{
	bsoncxx::builder::basic::document newSubdocument;
	newSubdocument.append(bsoncxx::builder::basic::kvp("FromConnectable", _connection.fromConnectable));
	newSubdocument.append(bsoncxx::builder::basic::kvp("ToConnectable", _connection.toConnectable));
	newSubdocument.append(bsoncxx::builder::basic::kvp("FromUID", _connection.fromUID));
	newSubdocument.append(bsoncxx::builder::basic::kvp("ToUID", _connection.toUID));
	return newSubdocument;
}

void ot::BlockConnection::DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage)
{
	_connection.fromConnectable = storage.view()["FromConnectable"].get_utf8().value.to_string();
	_connection.toConnectable = storage.view()["ToConnectable"].get_utf8().value.to_string();
	_connection.fromUID = storage.view()["FromUID"].get_utf8().value.to_string();
	_connection.toUID= storage.view()["ToUID"].get_utf8().value.to_string();
}

