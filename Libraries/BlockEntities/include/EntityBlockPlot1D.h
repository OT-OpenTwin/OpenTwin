#pragma once
#include "EntityBlock.h"
class __declspec(dllexport) EntityBlockPlot1D : public EntityBlock
{
public:
	EntityBlockPlot1D(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockPlot1D"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; }
	void createProperties();

	std::string getXLabel();
	std::string getYLabel();

	std::string getXUnit();
	std::string getYUnit();

	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;

	const ot::Connector getConnectorXAxis() const { return _xAxisConnector; }
	const ot::Connector getConnectorYAxis() const { return _yAxisConnector; }

private:
	ot::Connector _xAxisConnector;
	ot::Connector _yAxisConnector;

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};
