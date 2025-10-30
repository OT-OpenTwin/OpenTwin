// @otlicense
// File: ArrayBinaryConverter.h
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
namespace DataStorageAPI
{
	class __declspec(dllexport) ArrayBinaryConverter
	{
	public:
		template<typename T> static  bsoncxx::types::b_binary convertArrayToBinary(T arr[], const size_t size) {
			bsoncxx::types::b_binary bin_val{
			bsoncxx::binary_sub_type::k_binary, static_cast<std::uint32_t>(size * sizeof(T)), reinterpret_cast<std::uint8_t*>(arr) };
			return bin_val;
		}
		template<typename T> static  T* convertBinaryToArray(bsoncxx::types::b_binary data) {
			T* arrayRef = (T*)data.bytes;
			return arrayRef;
		}
	};
}