#include "BlockHandlerFileWriter.h"
#include "OTCore/OTAssert.h"
#include "OTCore/GenericDataStruct.h"
#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"

BlockHandlerFileWriter::BlockHandlerFileWriter(EntityBlockFileWriter* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity, handlerMap)
{
	m_input = blockEntity->getConnectorInput();
	m_fileName = blockEntity->getFileName();
	m_filePath = blockEntity->getFilePath();
	m_headline = blockEntity->getHeadline();
	const std::string& selectedFileMode = blockEntity->getSelectedFileMode();
	if (selectedFileMode == blockEntity->getFileModeAppend())
	{
		m_openMode = std::ios_base::app;
	}
	else if(selectedFileMode == blockEntity->getFileModeOverride())
	{
		m_openMode = std::ios_base::out;
	}
	else
	{
		assert(0);
	}
}

bool BlockHandlerFileWriter::executeSpecialized()
{
	_uiComponent->displayMessage("Executing Filewriter Block: " + _blockName);
	PipelineData& incommingPortData	= _dataPerPort[m_input.getConnectorName()];
	GenericDataList& dataList = incommingPortData.m_data;
	char lastCharacter = m_filePath[m_filePath.size()-1];
	if (lastCharacter != '\\' && lastCharacter != '/')
	{
		m_filePath = m_filePath + "/";
	}
	size_t  pointIndex = m_fileName.find('.');
	if (pointIndex == std::string::npos)
	{
		m_fileName = m_fileName + ".txt";
	}

	const std::string fullPath = m_filePath + m_fileName;
	if (m_fileStream.is_open())
	{
		m_fileStream.close();
	}
	m_fileStream.open(fullPath, m_openMode);

	if (!m_fileStream.is_open())
	{
		throw std::exception(std::string("Failed to open file for writing: " + fullPath).c_str());
	}
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
	m_fileStream.close();
	return true;
}

BlockHandlerFileWriter::~BlockHandlerFileWriter()
{
	m_fileStream.close();
}

void BlockHandlerFileWriter::streamVariable(std::ofstream& stream, const ot::Variable& value)
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
