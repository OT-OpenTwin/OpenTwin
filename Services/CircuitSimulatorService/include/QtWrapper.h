#pragma once
//std Header
#include <string>


class QCoreApplication;
class ConnectionManager;

class QtWrapper {
public:
	QtWrapper();
	
	void run(const std::string& _serverName);
private:
	QCoreApplication* m_application;
	ConnectionManager* m_connectionManager;
	
	void worker(std::string _serverName);
};