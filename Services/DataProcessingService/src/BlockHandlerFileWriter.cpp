#include "BlockHandlerFileWriter.h"
#include "OTCore/OTAssert.h"
#include "OTCore/GenericDataStruct.h"
#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "EntityFileText.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCore/FolderNames.h"
#include "EntityBinaryData.h"

BlockHandlerFileWriter::BlockHandlerFileWriter(EntityBlockFileWriter* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity, handlerMap)
{
	m_input = blockEntity->getConnectorInput();
	m_fileName = blockEntity->getFileName();
	m_headline = blockEntity->getHeadline();
}

bool BlockHandlerFileWriter::executeSpecialized()
{
	_uiComponent->displayMessage("Executing Filewriter Block: " + _blockName);
	PipelineData& incommingPortData	= _dataPerPort[m_input.getConnectorName()];
	GenericDataList& dataList = incommingPortData.m_data;


	m_fileStream << m_headline<<"\n";

	for (auto data : dataList)
	{
		ot::GenericDataStructSingle* single = dynamic_cast<ot::GenericDataStructSingle*>(data.get());
		if (single != nullptr)
		{
			streamVariable(m_fileStream, single->getValue());
			m_fileStream << "\n";
		}

		ot::GenericDataStructVector* vector = dynamic_cast<ot::GenericDataStructVector*>(data.get());
		if (vector != nullptr)
		{
			m_fileStream << "[";
			const std::vector<ot::Variable>& values = vector->getValues();
			const uint32_t numberOfEntries = vector->getNumberOfEntries();
			for (uint32_t index = 0; index < numberOfEntries; index++)
			{
				streamVariable(m_fileStream, values[index]);
				if (index != numberOfEntries - 1)
				{
					m_fileStream << ", ";
				}
			}
			m_fileStream << "]\n";
		}

		ot::GenericDataStructMatrix* matrix = dynamic_cast<ot::GenericDataStructMatrix*>(data.get());
		if (matrix != nullptr)
		{
			uint32_t rows = matrix->getNumberOfRows();
			uint32_t columns = matrix->getNumberOfColumns();
			ot::MatrixEntryPointer matrixEntry;
			for (matrixEntry.m_row = 0; matrixEntry.m_row < rows; matrixEntry.m_row++)
			{
				m_fileStream << "[";
				for (matrixEntry.m_column = 0; matrixEntry.m_column < columns; matrixEntry.m_column++)
				{
					const ot::Variable& var = matrix->getValue(matrixEntry);
					streamVariable(m_fileStream, var);
					if (matrixEntry.m_column != columns - 1)
					{
						m_fileStream << ", ";
					}
				}
				m_fileStream << "]\n";
			}
		}
	}
	
	createFile();

	return true;
}

void BlockHandlerFileWriter::createFile()
{
	const std::string fileName = CreateNewUniqueTopologyName(ot::FolderNames::FilesFolder, m_fileName);

	EntityFileText textFile(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService);
	textFile.setName(fileName);

	textFile.setFileProperties("", m_fileName, "txt");
	
	EntityBinaryData data(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService);
	const std::string fileContent = m_fileStream.str();

	data.setData(fileContent.c_str(), fileContent.size());
	data.StoreToDataBase();

	textFile.setData(data.getEntityID(),data.getEntityStorageVersion());
	textFile.StoreToDataBase();

	_modelComponent->addEntitiesToModel({ textFile.getEntityID() }, { textFile.getEntityStorageVersion() }, { false }, { data.getEntityID() }, { data.getEntityStorageVersion() }, { textFile.getEntityID() }, "Created text file.");
}

void BlockHandlerFileWriter::streamVariable(std::stringstream& stream, const ot::Variable& value)
{
	if (value.isBool())
	{
		stream << value.getBool();
	}
	else if(value.isConstCharPtr())
	{
		stream << value.getConstCharPtr();
	}
	else if (value.isDouble()) 
	{
		stream << value.getDouble();
	}
	else if (value.isFloat()) 
	{
		stream << value.getFloat();
	}
	else if (value.isInt32()) 
	{
		stream << value.getInt32();
	}
	else if (value.isInt64()) 
	{
		stream << value.getInt64();
	}
	else
	{
		OTAssert(0, "Not supported type");
	}

}
