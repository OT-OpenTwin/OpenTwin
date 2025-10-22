#pragma once
#include "BlockHandler.h"
#include "EntityBlockPython.h"
#include "OTServiceFoundation/PythonServiceInterface.h"

class BlockHandlerPython : public BlockHandler
{
public: 
	BlockHandlerPython(EntityBlockPython* blockEntity, const HandlerMap& handlerMap);
	virtual bool executeSpecialized() override;

	// Inherited via BlockHandler
	std::string getBlockType() const override;

private:
	std::list<std::string> m_requiredInput;
	std::list<std::string> m_outputs;
	std::list<PipelineData> m_outputData;
	std::string m_scriptName;
	std::string m_entityName;
	ot::PythonServiceInterface* m_pythonServiceInterface = nullptr;

};
