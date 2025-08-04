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

private:
	std::string m_name;
	std::list<Endpoint> m_endpoints;
};