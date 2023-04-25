#include "stdafx.h"
#include "..\include\Response\DataStorageResponse.h"

namespace DataStorageAPI
{

	void DataStorageResponse::UpdateDataStorageResponse(std::string result, bool success, std::string message)
	{
		_result = result;
		_success = success;
		_message = message;
	}
}
