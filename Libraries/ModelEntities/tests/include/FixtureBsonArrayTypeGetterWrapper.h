// @otlicense
// File: FixtureBsonArrayTypeGetterWrapper.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>

#include <string>
#include <vector>
#include <stdint.h>
#include "gtest/gtest.h"

class FixtureBsonArrayTypeGetterWrapper : public testing::Test
{
public:
	FixtureBsonArrayTypeGetterWrapper() {
		m_exampleDataUTF8String.append("a");
		m_exampleDataUTF8String.append("b");
		m_exampleDataUTF8String.append("c");
		m_exampleDataUTF8String.append("d");

		m_exampleDataChar.append('a');
		m_exampleDataChar.append('b');
		m_exampleDataChar.append('c');
		m_exampleDataChar.append('d');

		m_exampleDataInt32.append(int32_t(1));
		m_exampleDataInt32.append(int32_t(2));
		m_exampleDataInt32.append(int32_t(3));
		m_exampleDataInt32.append(int32_t(4));

		m_exampleDataInt64.append(int64_t(1));
		m_exampleDataInt64.append(int64_t(2));
		m_exampleDataInt64.append(int64_t(3));
		m_exampleDataInt64.append(int64_t(4));

		m_exampleDataDouble.append(double(1.0));
		m_exampleDataDouble.append(double(2.0));
		m_exampleDataDouble.append(double(3.0));
		m_exampleDataDouble.append(double(4.0));

		m_exampleDataFloat.append(float(1.0f));
		m_exampleDataFloat.append(float(2.0f));
		m_exampleDataFloat.append(float(3.0f));
		m_exampleDataFloat.append(float(4.0f));
	}

	const bsoncxx::v_noabi::array::view& GetUTF8StringArray() { return m_exampleDataUTF8String.view(); };
	const bsoncxx::v_noabi::array::view& GetCharArray() { return m_exampleDataChar.view(); };
	const bsoncxx::v_noabi::array::view& GetInt32Array() { return m_exampleDataInt32.view(); };
	const bsoncxx::v_noabi::array::view& GetInt64Array() { return m_exampleDataInt64.view(); };
	const bsoncxx::v_noabi::array::view& GetDoubleArray() { return m_exampleDataDouble.view(); };
	const bsoncxx::v_noabi::array::view& GetFloatArray() { return m_exampleDataFloat.view(); };
	~FixtureBsonArrayTypeGetterWrapper() {};

private:
	bsoncxx::builder::basic::array m_exampleDataUTF8String;
	bsoncxx::builder::basic::array m_exampleDataChar;
	bsoncxx::builder::basic::array m_exampleDataInt32;
	bsoncxx::builder::basic::array m_exampleDataInt64;
	bsoncxx::builder::basic::array m_exampleDataDouble;
	//Float is not supported and is being used as an example for the template default case.
	bsoncxx::builder::basic::array m_exampleDataFloat;

};
