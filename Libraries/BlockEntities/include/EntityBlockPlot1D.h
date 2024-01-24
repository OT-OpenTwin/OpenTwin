#pragma once
#include "EntityBlock.h"
class __declspec(dllexport) EntityBlockPlot1D : public EntityBlock
{
public:
	EntityBlockPlot1D(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockPlot1D"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; }
	virtual bool updateFromProperties() override;

	void createProperties();

	std::string getXLabel();
	std::string getYLabel();

	std::string getXUnit();
	std::string getYUnit();

	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;

	const ot::Connector& getConnectorXAxis() const { return _xAxisConnector; }
	const std::list<const ot::Connector*> getConnectorsYAxis();
	const std::list<std::string> getCurveNames();
	const int getNumberOfCurves();
private:
	ot::Connector _xAxisConnector;
	
	const std::string _propertyGroupYAxisDefinition = "Data properties";
	const std::string _propertyCurveNameBase = "Curve Name ";
	const int _numberOfConnectorsUnrelatedToCurves = 1;
	const std::string _connectorYAxisNameBase = "YAxis";
	const std::string _connectorYAxisTitleBase = "Y-Axis ";


	void AddDynamicNumberOfCurves(int numberOfCurves);

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};
