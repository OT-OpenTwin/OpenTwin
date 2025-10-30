// @otlicense

#pragma once
#include <map>
#include <functional>
#include <string>
#include <memory>
#include "TableExtractor.h"
#include <mutex>

//! @brief Singleton which the FileToTableExtractorRegistrar class uses to register a new file eding. Registered classes can be created via the Create function.
class FileToTableExtractorFactory
{
public:
	static FileToTableExtractorFactory* GetInstance();
	std::shared_ptr<TableExtractor> Create(std::string name) const;
	void RegisterFactoryFunction(std::string name, std::function<TableExtractor*(void)> classFactoryFunction);

private:
	std::map<std::string, std::function<TableExtractor*(void)>> factoryFunctionRegistry;
	std::mutex mtx;
	FileToTableExtractorFactory() {};
	~FileToTableExtractorFactory() {};
	FileToTableExtractorFactory(const FileToTableExtractorFactory& other) = delete;
	FileToTableExtractorFactory& operator=(const FileToTableExtractorFactory& other) = delete;
};