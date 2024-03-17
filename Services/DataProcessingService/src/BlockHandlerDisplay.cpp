#include "BlockHandlerDisplay.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructSingle.h"

BlockHandlerDisplay::BlockHandlerDisplay(EntityBlockDisplay* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(handlerMap)
{
	_input = blockEntity->getConnectorInput();
	_description = blockEntity->getDescription();
}

bool BlockHandlerDisplay::executeSpecialized()
{
	GenericDataList& dataList =	_dataPerPort[_input.getConnectorName()];
	std::string displayMessage = _description + "\n";

	for (auto data : dataList)
	{
		ot::GenericDataStructSingle* single = dynamic_cast<ot::GenericDataStructSingle*>(data.get());
		if (single != nullptr)
		{
			const std::string stringVal = variableToString(single->getValue());
			displayMessage += stringVal + "\n";
		}

		ot::GenericDataStructVector* vector = dynamic_cast<ot::GenericDataStructVector*>(data.get());
		if (vector != nullptr)
		{
			displayMessage += "[";
			const std::vector<ot::Variable>& values = vector->getValues();
			const uint32_t numberOfEntries = vector->getNumberOfEntries();
			for (uint32_t index = 0; index < numberOfEntries; index++)
			{
				const std::string stringVal = variableToString(values[index]);
				displayMessage += stringVal;
				if (index != numberOfEntries - 1)
				{
					displayMessage += ", ";
				}
			}
			displayMessage += "]\n";
		}

		ot::GenericDataStructMatrix* matrix = dynamic_cast<ot::GenericDataStructMatrix*>(data.get());
		if(matrix != nullptr)
		{
			uint32_t rows =	matrix->getNumberOfRows();
			uint32_t columns = matrix->getNumberOfColumns();
			
			for (uint32_t row = 0; row < rows; row++)
			{
				displayMessage += "[";
				for (uint32_t column= 0; column< columns; column++)
				{
					const ot::Variable& var = matrix->getValue(column, row);
					const std::string stringVal = variableToString(var);
					displayMessage += stringVal;
					if (column != columns - 1)
					{
						displayMessage += ", ";
					}
				}
				displayMessage += "]\n";
			}
		}
	}

	_uiComponent->displayMessage(displayMessage + "\n");
	return true;
}

std::string BlockHandlerDisplay::variableToString(const ot::Variable& var)
{
	if (var.isConstCharPtr())
	{
		return var.getConstCharPtr();
	}
	else if (var.isBool())
	{
		std::string retVal;
		var.getBool() ?  retVal = "TRUE" : retVal = "FALSE";
		return retVal;
	}
	else if (var.isDouble())
	{
		return std::to_string(var.getDouble());
	}
	else if (var.isFloat())
	{
		return std::to_string(var.getFloat());
	}
	else if (var.isInt32())
	{
		return std::to_string(var.getInt32());
	}
	else
	{
		assert(var.isInt64());
		return std::to_string(var.getInt64());
	}
}
