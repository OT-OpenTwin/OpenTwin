#pragma once
#include "BlockConnectionBSON.h"


ot::BlockConnectionBSON::BlockConnectionBSON(const ot::GraphicsConnectionCfg& connection)
	: _connection(connection)
{
}

const ot::GraphicsConnectionCfg& ot::BlockConnectionBSON::getConnection() const
{
	return _connection;
}

void ot::BlockConnectionBSON::setConnection(const ot::GraphicsConnectionCfg& connection)
{
	_connection = connection;
}

bsoncxx::builder::basic::document ot::BlockConnectionBSON::SerializeBSON() const
{
	bsoncxx::builder::basic::document newSubdocument;
	newSubdocument.append(bsoncxx::builder::basic::kvp("FromConnectable", _connection.originConnectable()));
	newSubdocument.append(bsoncxx::builder::basic::kvp("ToConnectable", _connection.destConnectable()));
	newSubdocument.append(bsoncxx::builder::basic::kvp("FromUID", _connection.originUid()));
	newSubdocument.append(bsoncxx::builder::basic::kvp("ToUID", _connection.destUid()));
	newSubdocument.append(bsoncxx::builder::basic::kvp("uid", static_cast<int64_t>(_connection.getUid())));
	return newSubdocument;
}

void ot::BlockConnectionBSON::DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage)
{
	_connection.setOriginConnectable(storage.view()["FromConnectable"].get_utf8().value.to_string());
	_connection.setDestConnectable(storage.view()["ToConnectable"].get_utf8().value.to_string());
	_connection.setOriginUid(storage.view()["FromUID"].get_utf8().value.to_string());
	_connection.setDestUid(storage.view()["ToUID"].get_utf8().value.to_string());
	_connection.setUid(static_cast<ot::UID>(storage.view()["uid"].get_int64()));
}

