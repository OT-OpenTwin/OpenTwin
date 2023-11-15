#pragma once
#include "BlockHandler.h"
#include "EntityBlockPython.h"
#include "OpenTwinFoundation/PythonServiceInterface.h"

class BlockHandlerPython : public BlockHandler
{
public: 
	BlockHandlerPython(EntityBlockPython* blockEntity, const HandlerMap& handlerMap);
	virtual bool executeSpecialized() override;

private:
	std::list<std::string> _requiredInput;
	std::string _scriptName;
	std::string _entityName;
	ot::PythonServiceInterface* _pythonServiceInterface = nullptr;
};
