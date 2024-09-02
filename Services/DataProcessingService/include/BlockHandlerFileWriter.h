#pragma once
#include "BlockHandler.h"
#include "EntityBlockFileWriter.h"
#include <fstream>

class BlockHandlerFileWriter : public BlockHandler
{
public:
	BlockHandlerFileWriter(EntityBlockFileWriter* blockEntity, const HandlerMap& handlerMap);
	virtual bool executeSpecialized() override;
	~BlockHandlerFileWriter();
private:
	ot::Connector m_input;
	std::string m_headline;
	std::string m_filePath;
	std::string m_fileName;
	std::ios_base::openmode m_openMode = std::ios_base::out;
	std::ofstream m_fileStream;

	void streamVariable(std::ofstream& stream, const ot::Variable& value);
};