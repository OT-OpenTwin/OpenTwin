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
	EntityBlockStorage(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
	virtual std::string getClassName(void) override { return "EntityBlockStorage"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }
	void createProperties();
	virtual bool updateFromProperties() override;

	int32_t getNumberOfQuantities();
	int32_t getNumberOfParameters();
	int32_t getNumberOfMetaData();

	static const std::string getIconName() { return "Database_store.svg"; }

	const std::list<ot::Connector> getConnectorsQuantity();
	const std::list<ot::Connector> getConnectorsParameter();
	const std::list<ot::Connector> getConnectorsMetadata();

	const ParameterProperties getPropertiesParameter(const std::string& _parameterName) ;
	const QuantityProperties getPropertiesQuantity(const std::string& _quantityName) ;

	std::string getSeriesName();
private:	
	std::list<ot::Connector*> m_quantityInputs;
	const std::string m_quantityInputNameBase = "Quantity";
	std::list<ot::Connector*> m_parameterInputs;
	const std::string m_parameterInputNameBase = "Parameter";
	std::list<ot::Connector*> m_metaDataInputs;
	const std::string m_metaDataInputNameBase = "Meta data";

	const std::string m_propertyNbOfQuantities = "Number of quantities";
	const std::string m_propertyNbOfParameter = "Number of parameters";
	const std::string m_propertyNbOfMetaData = "Number of meta data";
	const std::string m_groupGeneral = "General";

	const std::string m_propertyConstParameter = "Is constant";
	const std::string m_propertyName = "Name";
	const std::string m_propertyUnit = "Unit";
	const std::string m_propertyType = "Type";

	void createParameterProperties(const std::string& _groupName);

	void createConnectors();
	void clearConnectors();
};
