#pragma once
#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockDataDimensionReducer : public EntityBlock
{
public:
	EntityBlockDataDimensionReducer(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockDataDimensionReducer"; };
	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;

	void createProperties();

	int getInputRow() ;
	int getInputColumn();

	const ot::Connector* getInputConnector() const { return &_inputConnector; }
	const ot::Connector* getOutputConnector() const { return &_outputConnector; }
private:
	ot::Connector _inputConnector;
	ot::Connector _outputConnector;

};
