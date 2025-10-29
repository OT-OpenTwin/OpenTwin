// @otlicense

#pragma once
#include "BsonArrayTypeGetterWrapper.h"


template<class T>
T BsonArrayTypeGetterWrapper<T>::getValue(const bsoncxx::v_noabi::array::element& bsonArrayElement)
{
	throw std::exception("Type is not supported by this wrapper");
}


template<>
inline int32_t BsonArrayTypeGetterWrapper<int32_t>::getValue(const bsoncxx::v_noabi::array::element& bsonArrayElement)
{
	return bsonArrayElement.get_int32();
}

template<>
inline int64_t BsonArrayTypeGetterWrapper<int64_t>::getValue(const bsoncxx::v_noabi::array::element& bsonArrayElement)
{
	return bsonArrayElement.get_int64();
}

template<>
inline std::string BsonArrayTypeGetterWrapper<std::string>::getValue(const bsoncxx::v_noabi::array::element& bsonArrayElement)
{
	return bsonArrayElement.get_utf8().value.data();
}

template<>
inline double BsonArrayTypeGetterWrapper<double>::getValue(const bsoncxx::v_noabi::array::element& bsonArrayElement)
{
	return bsonArrayElement.get_double();
}