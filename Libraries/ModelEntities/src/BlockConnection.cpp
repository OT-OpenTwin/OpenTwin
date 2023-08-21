#pragma once
#include "BlockConnection.h"

ot::BlockConnection::BlockConnection(const std::string& identifierFrom, const std::string& identifierTo)
	:_identifierFrom(identifierFrom), _identifierTo(identifierTo)
{}

ot::BlockConnection::BlockConnection()
	:_identifierFrom(""), _identifierTo("")
{
}

std::string ot::BlockConnection::getIdentifierFrom() const
{
	return _identifierFrom;
}

std::string ot::BlockConnection::getIdentifierTo() const
{
	return _identifierTo;
}

bsoncxx::builder::basic::document ot::BlockConnection::SerializeBSON() const
{
	bsoncxx::builder::basic::document newSubdocument;
	newSubdocument.append(bsoncxx::builder::basic::kvp("identifierFrom", _identifierFrom));
	newSubdocument.append(bsoncxx::builder::basic::kvp("identifierTo", _identifierTo));
	return newSubdocument;
}

void ot::BlockConnection::DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage)
{
	_identifierFrom = storage.view()["identifierFrom"].get_utf8().value.to_string();
	_identifierTo = storage.view()["identifierTo"].get_utf8().value.to_string();
}
