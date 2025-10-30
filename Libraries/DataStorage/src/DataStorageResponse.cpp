// @otlicense
// File: DataStorageResponse.cpp
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
