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
		__declspec(dllexport) void UpdateDataStorageResponse(std::string result, bool success, std::string message);

		__declspec(dllexport) std::string getResult() { return _result; }
		__declspec(dllexport) void setResult(std::string result) { _result = result; };

		__declspec(dllexport) std::string getMessage() { return _message; };
		__declspec(dllexport) void setMessage(std::string message) { _message = message; };

		__declspec(dllexport) bool getSuccess() { return _success; };
		__declspec(dllexport) void setSuccess(bool success) { _success = success; };

		__declspec(dllexport) bsoncxx::stdx::optional<bsoncxx::document::value> getBsonResult() { return _bsonResult; };
		__declspec(dllexport) void setBsonResult(bsoncxx::stdx::optional<bsoncxx::document::value> value) { _bsonResult = value; };

		__declspec(dllexport) std::string getFilePath() { return _filePath; };
		__declspec(dllexport) void setFilePath(std::string filePath) { _filePath = filePath; };

		__declspec(dllexport) int getInsertType() { return _insertType; };
		__declspec(dllexport) void setInsertType(int insertType) { _insertType = insertType; };

	private:
		std::string _result;
		bool _success;
		std::string _message;
		bsoncxx::stdx::optional<bsoncxx::document::value> _bsonResult;
		std::string _filePath;
		int _insertType;
	};
}

