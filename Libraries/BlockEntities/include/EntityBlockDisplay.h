#pragma once
#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockDisplay : public EntityBlock
{
public:
	EntityBlockDisplay() : EntityBlockDisplay(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockDisplay(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);

	static std::string className() { return "EntityBlockDisplay"; }
	virtual std::string getClassName(void) const override { return EntityBlockDisplay::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }
		
	void createProperties();
	const std::string& getDescription();
	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	const ot::Connector& getConnectorInput() const { return _inputConnector; }

	static const std::string getIconName() { return "Monitor.svg"; }

private:
	ot::Connector _inputConnector;
};
