#pragma once
#include "EntityBlock.h"
class __declspec(dllexport) EntityBlockStorage : public EntityBlock
{
public:
	EntityBlockStorage(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
	virtual std::string getClassName(void) override { return "EntityBlockStorage"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; }
	void createProperties();
	virtual bool updateFromProperties() override;

	int32_t getNumberOfQuantities();
	int32_t getNumberOfParameters();
	int32_t getNumberOfMetaData();

	static const std::string getIconName() { return "Database_store.svg"; }
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

	void createParameterProperties(const std::string& _groupName);

	void createConnectors();
	void clearConnectors();
};
