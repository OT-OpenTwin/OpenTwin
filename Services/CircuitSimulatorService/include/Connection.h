// @otlicense
// File: Connection.h
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

//C++ Header
#include <string>

//Service Header

//Ot Header
#include "OTGui/GraphicsConnectionCfg.h"

class Connection : public ot::GraphicsConnectionCfg
{
public:
	Connection();
	virtual ~Connection();

	Connection(const GraphicsConnectionCfg& obj);

	/*bool operator<(const Connection& other) const {
		if (this->getOriginUid() == other.getOriginUid()) {
			return this->getDestinationUid() < other.getDestinationUid();
		}
		else {
			return this->getOriginUid() < other.getOriginUid();
		}
	}*/

	/*bool operator==(const Connection& other) const {
		return this->getOriginUid() != other.getOriginUid() && this->getDestinationUid() != other.getDestinationUid();
	}*/

	void setNodeNumber(std::string num)
	{
		this->nodeNumber = num;
	}

	std::string getNodeNumber()
	{
		return this->nodeNumber;
	}

private:
	
	std::string nodeNumber;

	


};