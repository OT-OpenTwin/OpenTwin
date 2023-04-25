#pragma once
#include "FixtureBsonArrayTypeGetterWrapper.h"



bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::GetUTF8StringArray()
{
	return CreateArrayView<std::string>(exampleDataUTF8String);
}

bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::GetCharArray()
{
	return bsoncxx::v_noabi::array::view();
}

bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::GetInt32Array()
{
	return CreateArrayView<int32_t>(exampleDataInt32);
}

bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::GetInt64Array()
{
	return CreateArrayView<int64_t>(exampleDataInt64);
}

bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::GetDoubleArray()
{
	return CreateArrayView<double>(exampleDataDouble);
}

bsoncxx::v_noabi::array::view FixtureBsonArrayTypeGetterWrapper::GetFloatArray()
{
	return CreateArrayView<float>(exampleDataFloat);
}
