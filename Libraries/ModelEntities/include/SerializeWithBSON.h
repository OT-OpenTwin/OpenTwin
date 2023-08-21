#pragma once
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

class __declspec(dllexport) SerializeWithBSON
{
public:
	virtual bsoncxx::builder::basic::document SerializeBSON() const = 0 ;
	virtual void DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage) = 0;
	void DeserializeBSON(bsoncxx::v_noabi::types::bson_value::view& storage)
	{
		auto subDocument = storage.get_document();
		DeserializeBSON(subDocument);
	};
};
