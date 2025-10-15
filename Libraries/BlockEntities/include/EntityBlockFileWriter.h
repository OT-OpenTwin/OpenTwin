#pragma once
#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockFileWriter : public EntityBlock
{
public:
	EntityBlockFileWriter() : EntityBlockFileWriter(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockFileWriter(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);

	static std::string className() { return "EntityBlockFileWriter"; }
	virtual std::string getClassName(void) override { return EntityBlockFileWriter::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	void createProperties();
	const std::string& getHeadline();
	const std::string& getFileName();
	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	const ot::Connector& getConnectorInput() const { return m_inputConnector; }
	const std::string& getFileModeAppend() const { return m_fileModeAppend; }
	const std::string& getFileModeOverride() const { return m_fileModeOverride; }

	static std::string getIconName(){ return "TextFile.svg"; }
	const std::string getFileType();

private:
	ot::Connector m_inputConnector;
	const std::string m_fileModeAppend = "Append";
	const std::string m_fileModeOverride = "Override";

};
