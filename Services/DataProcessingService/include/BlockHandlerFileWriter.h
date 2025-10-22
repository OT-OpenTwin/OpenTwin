#pragma once
#include "BlockHandler.h"
#include "EntityBlockFileWriter.h"
#include <sstream>

class BlockHandlerFileWriter : public BlockHandler
{
public:
	BlockHandlerFileWriter(EntityBlockFileWriter* blockEntity, const HandlerMap& handlerMap);
	virtual bool executeSpecialized() override;
	
	std::string getBlockType() const override;
private:
	ot::Connector m_input;
	std::string m_headline;
	std::string m_fileName;
	std::string m_fileType;

	std::stringstream m_fileStream;
	void createFile();

};