#include "BlockHandlerFileWriter.h"
#include "OTCore/VariableToStringConverter.h"
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
	GenericDataList& dataList = _dataPerPort[m_input.getConnectorName()];
	ot::VariableToStringConverter converter;
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
			const std::string stringVal = converter(single->getValue());
			m_fileStream << stringVal + "\n";
		}

		ot::GenericDataStructVector* vector = dynamic_cast<ot::GenericDataStructVector*>(data.get());
		if (vector != nullptr)
		{
			m_fileStream << "[";
			const std::vector<ot::Variable>& values = vector->getValues();
			const uint32_t numberOfEntries = vector->getNumberOfEntries();
			for (uint32_t index = 0; index < numberOfEntries; index++)
			{
				const std::string stringVal = converter(values[index]);
				m_fileStream << stringVal;
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

			for (uint32_t row = 0; row < rows; row++)
			{
				m_fileStream << "[";
				for (uint32_t column = 0; column < columns; column++)
				{
					const ot::Variable& var = matrix->getValue(column, row);
					const std::string stringVal = converter(var);
					m_fileStream << stringVal;
					if (column != columns - 1)
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
