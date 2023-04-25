#include "FileToTableExtractorFactory.h"



void FileToTableExtractorFactory::RegisterFactoryFunction(std::string name, std::function<TableExtractor*(void)> classFactoryFunction)
{
	mtx.lock();
	factoryFunctionRegistry[name] = classFactoryFunction;
	mtx.unlock();
}

//Meyers Singleton and as Blockscope instantiated static, threadsafe by default.
FileToTableExtractorFactory * FileToTableExtractorFactory::GetInstance()
{
	static FileToTableExtractorFactory instance;
	return &instance;
}

std::shared_ptr<TableExtractor> FileToTableExtractorFactory::Create(std::string name) const
{
		TableExtractor* instance = nullptr;

		// find name in the registry and call factory method.
		auto it = factoryFunctionRegistry.find(name);
		if (it != factoryFunctionRegistry.end())
			instance = it->second();

		// wrap instance in a shared ptr and return
		if (instance != nullptr)
			return std::shared_ptr<TableExtractor>(instance);
		else
			return nullptr;
	
}
