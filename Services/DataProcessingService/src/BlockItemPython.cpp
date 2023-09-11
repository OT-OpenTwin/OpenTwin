#include "BlockItemPython.h"
#include "ExternalDependencies.h"
#include "BlockEntityFactoryRegistrar.h"



BlockItemPython::BlockItemPython()
	:BlockItemDataProcessing()
{
	_colourTitle.set(ot::Color::Cyan);
	_blockName = "Python";
	_blockTitle = "Python";
}

void BlockItemPython::AddConnectors(ot::GraphicsFlowItemCfg* block)
{
	block->addInput("C0", "Parameter", ot::GraphicsFlowConnectorCfg::Square);
	block->addOutput("C0", "Output", ot::GraphicsFlowConnectorCfg::Square);
}

std::shared_ptr<EntityBlock> BlockItemPython::CreateBlockEntity()
{
	std::shared_ptr<EntityBlockPython> pythonEnt (new EntityBlockPython(0, nullptr, nullptr, nullptr, nullptr, ""));
	ExternalDependencies dependency;
	pythonEnt->createProperties(ot::FolderNames::PythonScriptFolder,dependency.getPythonScriptFolderID());
	pythonEnt->AddConnector(ot::Connector(ot::ConnectorType::Filter, "C0"));
	pythonEnt->AddConnector(ot::Connector(ot::ConnectorType::Filter, "C1"));
	return pythonEnt;
}

static BlockEntityFactoryRegistrar factoryEntry("Python", BlockItemPython::CreateBlockEntity);