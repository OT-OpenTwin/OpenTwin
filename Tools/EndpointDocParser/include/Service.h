// @otlicense
// File: Service.h
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

// project header
#include "Endpoint.h"

// std header
#include <string>
#include <list>

class Service {
public:
	Service() = default;
	Service(const Service& _other) = default;
	Service(Service&& _other) noexcept = default;

	~Service() = default;

	Service& operator = (const Service& _other) = default;
	Service& operator = (Service&& _other) noexcept = default;

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName() const { return m_name; };

	void setEndpoints(const std::list<Endpoint>& _endpoints) { m_endpoints = _endpoints; };
	const std::list<Endpoint>& getEndpoints() const { return m_endpoints; };

	void addEndpoint(const Endpoint& _endpoint) { m_endpoints.push_back(_endpoint); };

	void printService() const;

private:
	std::string m_name;
	std::list<Endpoint> m_endpoints;
};