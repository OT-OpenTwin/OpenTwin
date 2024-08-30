#pragma once
#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockFileWriter : public EntityBlock
{
public:
	EntityBlockFileWriter(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockFileWriter"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; }

	void createProperties();
	const std::string& getHeadline();
	const std::string& getSelectedFileMode();
	const std::string& getFileName();
	const std::string& getFilePath();
	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
	const ot::Connector& getConnectorInput() const { return m_inputConnector; }
	const std::string& getFileModeAppend() const { return m_fileModeAppend; }
	const std::string& getFileModeOverride() const { return m_fileModeOverride; }

	static std::string getIconName(){ return "TextFile.svg"; }

private:
	ot::Connector m_inputConnector;
	const std::string m_fileModeAppend = "Append";
	const std::string m_fileModeOverride = "Override";

};
