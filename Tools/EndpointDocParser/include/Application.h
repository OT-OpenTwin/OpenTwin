#pragma once

// projct header
#include "Service.h"

// std header
#include <list>
#include <map>

class Application {
public:
	enum ParameterType{
		FunctionParam,
		ReturnParam
	};

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
	int run(void);

	bool searchForServices(void);

	bool searchIncludeAndSrcDirectoryFiles(const std::string& _file, Service& _service);

	bool searchSrcDirectoryFiles(const std::string& _srcDirectory, Service& _service);

	bool searchIncludeDirectoryFiles(const std::string& _includeDirectory, Service& _service);

	bool parseFile(const std::string& _file, Service& _service);

	bool startsWith(const std::string& _line, const std::string& _prefix);

	bool parseParameter(Parameter& _parameter, const std::string& _param, Endpoint& _endpoint, ParameterType _parameterType);

	void addService(const Service& _service);

	void importActionTypes(void);

	void parseMacroDefinition(const std::string& _content);

	void addDescriptionToLastParameter(std::list<Parameter>& _paramList, const std::string& _description);
	
private:
	std::list<Service> m_services;
	std::map<std::string, std::string> m_actionMacros;
};