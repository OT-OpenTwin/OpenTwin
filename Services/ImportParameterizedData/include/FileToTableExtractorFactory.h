/*****************************************************************//**
 * \file   FileToTableExtractorFactory.h
 * \brief  A Meyer's singleton which the FileToTableExtractorRegistrar class uses to register a new file eding. Registered classes can be created via the Create function.
 * 
 * \author Wagner
 * \date   April 2023
 *********************************************************************/
#pragma once
#include <map>
#include <functional>
#include <string>
#include <memory>
#include "TableExtractor.h"
#include <mutex>

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