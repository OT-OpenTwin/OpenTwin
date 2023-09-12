#include "BSONToVariableConverter.h"

ot::Variable BSONToVariableConverter::operator()(const bsoncxx::v_noabi::document::element& element)
{
	auto type = element.type();
	switch (type)
	{
		case bsoncxx::type::k_double:
		{
			return ot::Variable(element.get_double());
		}
		case bsoncxx::type::k_utf8:
		{
			return ot::Variable(element.get_utf8().value.data());
		}
		case bsoncxx::type::k_int32:
		{
			return ot::Variable(element.get_int32());
		}
		case bsoncxx::type::k_int64:
		{
			return ot::Variable(element.get_int64());
		}
		default:
		{
			throw std::exception("Not supported BSON to ot::Variable conversion");
		}
	}
}

ot::Variable BSONToVariableConverter::operator()(const bsoncxx::v_noabi::array::element& element)
{
	auto type = element.type();
	switch (type)
	{
		case bsoncxx::type::k_double:
		{
			return ot::Variable(element.get_double());
		}
		case bsoncxx::type::k_utf8:
		{
			return ot::Variable(element.get_utf8().value.data());
		}
		case bsoncxx::type::k_int32:
		{
			return ot::Variable(element.get_int32());
		}
		case bsoncxx::type::k_int64:
		{
			return ot::Variable(element.get_int64());
		}
		default:
		{
			throw std::exception("Not supported BSON to ot::Variable conversion");
		}
	};
}
