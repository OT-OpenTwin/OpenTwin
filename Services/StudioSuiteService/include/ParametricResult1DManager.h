#pragma once

class Result1DManager;
class Application;

#include <string>
#include <list>

class ParametricResult1DManager
{
public:
	ParametricResult1DManager(Application* app);
	ParametricResult1DManager() = delete;
	~ParametricResult1DManager();

	void clear();
	void add(Result1DManager& result1DManager);

private:
	std::string determineRunIDLabel(std::list<int>& runIDList);
	void processCurves(const std::string& category, const std::string& runIDLabel, std::list<int>& runIDList, Result1DManager& result1DManager);
	void parseAxisLabel(const std::string& value, std::string& label, std::string& unit);

	std::string resultFolderName;
	Application* application;
};
