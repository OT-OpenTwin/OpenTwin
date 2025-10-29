// @otlicense

﻿#pragma once
#include <bsoncxx/builder/basic/array.hpp>
#include <exception>
#include <stdint.h>
#include <string>

template<class T>
class BsonArrayTypeGetterWrapper
{
public:
	static T getValue(const bsoncxx::v_noabi::array::element& bsonArrayElement);
};

#include "BsonArrayTypeGetterWrapper.hpp"
