#pragma once
#include "EntityBlock.h"
#include "OTCore/ValueComparisionDefinition.h"

struct __declspec(dllexport) ValueCharacteristicProperties
{
	EntityPropertiesString* m_unit;
	EntityPropertiesString* m_dataType;
	EntityPropertiesSelection* m_label;
};

class __declspec(dllexport)  EntityBlockDatabaseAccess : public EntityBlock
{
public:
	EntityBlockDatabaseAccess(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockDatabaseAccess"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	void createProperties(std::list<std::string>& comparators);
	void setSelectionSeries(std::list<std::string>& _options, const std::string& _selectedValue);

	std::string getSelectedProjectName();

	EntityPropertiesSelection* getSeriesSelection();
	EntityPropertiesSelection* getQuantityValueDescriptionSelection();
	
	ValueCharacteristicProperties getQuantityValueCharacteristic();
	ValueCharacteristicProperties getValueCharacteristics(const std::string& _groupName);
	
	const ValueComparisionDefinition getSelectedQuantityDefinition();

	const ot::Connector getConnectorQuantity() const { return m_connectorOutput; }	

	virtual bool updateFromProperties() override;

	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;

	static const std::string getIconName() {return "Database_access.svg";}
protected:
	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:

	const std::string m_propertyNameProjectName = "Projectname";
	const std::string m_propertyNameSeriesMetadata = "Measurement series";

	const std::string m_groupQuerySettings = "Query settings";
	const std::string m_groupQuantitySetttings = "Quantity settings";

	const std::string m_propertyName = "Name";
	const std::string m_propertyValueDescription = "Type";
	const std::string m_propertyDataType = "Data type";
	const std::string m_propertyComparator = "Comparator";
	const std::string m_propertyValue = "Value";
	const std::string m_propertyUnit = "Unit";

	ot::Connector m_connectorOutput;

	void createUpdatedProperty(const std::string& _propName, const std::string& _propGroup, const std::string& _labelValue, EntityProperties& properties);

	const ValueComparisionDefinition getSelectedValueComparisionDefinition(const std::string& _groupName);

	bool setVisibleParameter(const std::string& _groupName,bool _visible);
	void updateBlockConfig();
	void updateConnections(std::list<std::string>& connectorsForRemoval);
	void removeConnectionsAtConnectedEntities(std::list<ot::UID>& connectionsForRemoval);

};
