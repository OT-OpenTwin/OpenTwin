// @otlicense

#include "stdafx.h"
#include "..\include\Response\DataStorageResponse.h"

namespace DataStorageAPI
{

	void DataStorageResponse::updateDataStorageResponse(const std::string& _result, bool _success, const std::string& _message)
	{
		m_result = _result;
		m_success = _success;
		m_message = _message;
	}
	void DataStorageResponse::updateDataStorageResponse(std::string&& _result, bool _success, std::string&& _message)
	{
		m_result = std::move(_result);
		m_success = std::move(_success);
		m_message = std::move(_message);
	}
}
