#pragma once

// projct header
#include "Service.h"

// std header
#include <list>

class Application {
public:
	// Constructors
	Application() = default;
	Application(const Application& _other) = default;
	Application(Application&& _other) noexcept = default;

	~Application() = default;

	Application& operator = (const Application& _other) = default;
	Application& operator = (Application&& _other) noexcept = default;

	// Getter, Setter
	void setServices(const std::list<Service>& _services) { m_services = _services; };
	const std::list<Service>& getServices() const { return m_services; };

	// Other methods
	void run(void);

	void searchForServices(void);

	void searchIncludeAndSrcDirectoryFiles(const std::string& _file);

	void searchSrcDirectoryFiles(const std::string& _srcDirectory);

	void searchIncludeDirectoryFiles(const std::string& _includeDirectory);

private:
	std::list<Service> m_services;
};