// @otlicense
// File: serviceInitializer.h
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

#include <string>
#include <list>

class ServiceInfo;
class Session;
namespace std { class thread; }

class ServiceInitializer {
public:
	ServiceInitializer() {}
	virtual ~ServiceInitializer();

	static void add(
		const std::string &	_serviceURL,
		const std::string &	_sessionID
	);

	static void run(void);

	static std::list<ServiceInfo *> data(void) { return m_services(); }
	static void setData(std::list<ServiceInfo *> _list) { m_services() = _list; }

	static void clean(void);

	static void setExiting(void);

	static bool isExiting(void) { return m_exiting(); }

	static bool isStartupComplete(void);

private:
	static std::list<ServiceInfo *> & m_services() { static std::list<ServiceInfo *> m_services; return m_services; };

	static std::thread*& m_thread() { static std::thread * m_thread{ nullptr }; return m_thread; }

	static bool & m_exiting() { static bool m_exiting{ false }; return m_exiting; }

};

class ServiceInfo {
public:
	ServiceInfo(
		const std::string &		_ip,
		const std::string &		_sessionID,
		int						_counter = 20
	) : m_counter{ _counter }, m_url{ _ip }, m_sessionID{ _sessionID }, m_startupComplete(false) {}

	virtual ~ServiceInfo() {}

	int decr(void) { return --m_counter; }

	void clearCounter(void) { m_counter = 0; }

	std::string url(void) const { return m_url; }

	std::string getSessionID(void) const { return m_sessionID; }

	void setStartupComplete(bool flag) { m_startupComplete = flag; }
	bool getStartupComplete(void) { return m_startupComplete; }

private:
	std::string			m_sessionID;
	int					m_counter;
	std::string			m_url;
	bool				m_startupComplete;

};
