#include <cassert>

#include "ClassFactoryBlock.h"
#include "EntityBlock.h"
#include "EntityBlockDatabaseAccess.h"
#include "EntityBlockPython.h"
#include "EntityBlockCircuitVoltageSource.h"
#include "EntityBlockCircuitResistor.h"
#include "EntityBlockDataDimensionReducer.h"
#include "EntityBlockStorage.h"
#include "EntityBlockConnection.h"
#include "EntityBlockDisplay.h"
#include "EntityBlockCircuitDiode.h"
#include "EntityBlockCircuitVoltageMeter.h"
#include "EntityBlockCircuitCurrentMeter.h"
#include "EntityBlockCircuitCapacitor.h"
#include "EntityBlockCircuitInductor.h"
#include "EntityBlockFileWriter.h"
#include "EntityBlockCircuitGND.h"
#include "EntityBlockCircuitConnector.h"
#include "EntityBlockCircuitTransmissionLine.h"

EntityBase* ClassFactoryBlock::CreateEntity(const std::string& entityType)
{

	if (entityType == "EntityBlockDatabaseAccess")
	{
		return new EntityBlockDatabaseAccess(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockPython")
	{
		return new EntityBlockPython(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockCircuitVoltageSource")
	{
		return new EntityBlockCircuitVoltageSource(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockCircuitResistor")
	{
		return new EntityBlockCircuitResistor(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockDataDimensionReducer")
	{
		return new EntityBlockDataDimensionReducer(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockStorage")
	{
		return new EntityBlockStorage(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockConnection")
	{
		return new EntityBlockConnection(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockDisplay")
	{
		return new EntityBlockDisplay(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockCircuitDiode")
	{
		return new EntityBlockCircuitDiode(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockCircuitVoltageMeter")
	{
		return new EntityBlockCircuitVoltageMeter(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockCircuitCurrentMeter")
	{
		return new EntityBlockCircuitCurrentMeter(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockCircuitCapacitor")
	{
		return new EntityBlockCircuitCapacitor(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockCircuitInductor")
	{
		return new EntityBlockCircuitInductor(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockFileWriter")
	{
		return new EntityBlockFileWriter(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockCircuitGND")
	{
		return new EntityBlockCircuitGND(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockCircuitConnector")
	{
		return new EntityBlockCircuitConnector(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockCircuitTransmissionLine") {
		return new EntityBlockCircuitTransmissionLine(0, nullptr, nullptr, nullptr, this, "");
	}

	return ClassFactoryHandlerAbstract::CreateEntity(entityType);
}
