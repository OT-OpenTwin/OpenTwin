// @otlicense
// File: DataStorageResponse.h
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
#include <iostream>
#include <string>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/cursor.hpp>
#include "../Enums/InsertTypeEnum.h"

namespace DataStorageAPI
{
	class DataStorageResponse
	{
	public:
		__declspec(dllexport) void updateDataStorageResponse(const std::string& _result, bool _success, const std::string& _message);
		__declspec(dllexport) void updateDataStorageResponse(std::string&& _result, bool _success, std::string&& _message);

		__declspec(dllexport) const std::string& getResult() { return m_result; }
		__declspec(dllexport) void setResult(const std::string& _result) { m_result = _result; };

		__declspec(dllexport) const std::string& getMessage() { return m_message; };
		__declspec(dllexport) void setMessage(std::string message) { m_message = message; };

		__declspec(dllexport) bool getSuccess() { return m_success; };
		__declspec(dllexport) void setSuccess(bool success) { m_success = success; };

		__declspec(dllexport) bsoncxx::stdx::optional<bsoncxx::document::value> getBsonResult() { return m_bsonResult; };
		__declspec(dllexport) void setBsonResult(bsoncxx::stdx::optional<bsoncxx::document::value> _value) { m_bsonResult = _value; };

		__declspec(dllexport) std::string getFilePath() { return m_filePath; };
		__declspec(dllexport) void setFilePath(const std::string& _filePath) { m_filePath = _filePath; };

		__declspec(dllexport) int getInsertType() { return m_insertType; };
		__declspec(dllexport) void setInsertType(int _insertType) { m_insertType = _insertType; };

	private:
		std::string m_result;
		bool m_success;
		std::string m_message;
		bsoncxx::stdx::optional<bsoncxx::document::value> m_bsonResult;
		std::string m_filePath;
		int m_insertType;
	};
}

