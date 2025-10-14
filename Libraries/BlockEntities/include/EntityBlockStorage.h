#pragma once
#include "EntityBlock.h"

struct __declspec(dllexport) ParameterProperties
{
	bool m_propertyConstParameter = false;
	std::string m_propertyName = "";
	std::string m_propertyUnit = "";
};

struct __declspec(dllexport) QuantityProperties
{
	std::string m_propertyType = "";
	std::string m_propertyName = "";
	std::string m_propertyUnit = "";
};

class __declspec(dllexport) EntityBlockStorage : public EntityBlock
{
public:
	EntityBlockStorage() : EntityBlockStorage(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockStorage(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);
	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
	virtual std::string getClassName(void) override { return "EntityBlockStorage"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }
	void createProperties();
	virtual bool updateFromProperties() override;

	uint32_t getNumberOfInputs();
	static const std::string getIconName() { return "Database_store.svg"; }

	bool getCreatePlot();
	std::string getPlotName();

	const std::list<std::string> getInputNames();
	const std::string getSeriesConnectorName() { return "SeriesMetadata"; }
private:	
	uint32_t m_maxNbOfInputs = 40;

	void createParameterProperties(const std::string& _groupName);

	void createConnectors();
	void clearConnectors();
};
