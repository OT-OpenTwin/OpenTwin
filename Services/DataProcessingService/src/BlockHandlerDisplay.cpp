#include "BlockHandlerDisplay.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/VariableToStringConverter.h"

BlockHandlerDisplay::BlockHandlerDisplay(EntityBlockDisplay* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity ,handlerMap)
{
	_input = blockEntity->getConnectorInput();
	_description = blockEntity->getDescription();
}

bool BlockHandlerDisplay::executeSpecialized()
{
	_uiComponent->displayMessage("Executing Display Block: " + _blockName);
	PipelineData& incommingPortData = _dataPerPort[_input.getConnectorName()];
	GenericDataList& dataList = incommingPortData.m_data;
	std::string displayMessage = _description + "\n";
	ot::VariableToStringConverter converter;
	for (auto data : dataList)
	{
		ot::GenericDataStructSingle* single = dynamic_cast<ot::GenericDataStructSingle*>(data.get());
		if (single != nullptr)
		{
			const std::string stringVal = converter(single->getValue());
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
				const std::string stringVal = converter(values[index]);
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

			ot::MatrixEntryPointer matrixEntry;
			for (matrixEntry.m_row= 0; matrixEntry.m_row < rows; matrixEntry.m_row++)
			{
				displayMessage += "[";
				for (matrixEntry.m_column = 0; matrixEntry.m_column < columns; matrixEntry.m_column++)
				{
					const ot::Variable& var = matrix->getValue(matrixEntry);
					const std::string stringVal = converter(var);
					displayMessage += stringVal;
					if (matrixEntry.m_column != columns - 1)
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
