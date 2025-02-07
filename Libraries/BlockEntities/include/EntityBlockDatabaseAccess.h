#pragma once
#include "EntityBlock.h"
#include "ValueComparisionDefinition.h"

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
	std::string getQueryDimension();

	const bool isQueryDimension1D();
	const bool isQueryDimension2D();
	const bool isQueryDimension3D();

	EntityPropertiesSelection* getSeriesSelection();
	EntityPropertiesSelection* getQuantityValueDescriptionSelection();
	
	ValueCharacteristicProperties getParameterValueCharacteristic1();
	ValueCharacteristicProperties getParameterValueCharacteristic2();
	ValueCharacteristicProperties getParameterValueCharacteristic3();
	ValueCharacteristicProperties getQuantityValueCharacteristic();
	ValueCharacteristicProperties getValueCharacteristics(const std::string& _groupName);
	
	const ValueComparisionDefinition getSelectedQuantityDefinition();
	const ValueComparisionDefinition getSelectedParameter1Definition();
	const ValueComparisionDefinition getSelectedParameter2Definition();
	const ValueComparisionDefinition getSelectedParameter3Definition();

	const ot::Connector getConnectorQuantity() const { return m_connectorQuantity; }
	const ot::Connector getConnectorParameter1() const { return m_connectorParameter1; }
	const ot::Connector getConnectorParameter2() const { return m_connectorParameter2; }
	const ot::Connector getConnectorParameter3() const { return m_connectorParameter3; }

	virtual EntityBase* clone() override;

	virtual bool updateFromProperties() override;

	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;

	static const std::string getIconName() {return "Database_access.svg";}
protected:
	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:

	const std::string m_propertyNameProjectName = "Projectname";

	const std::string m_propertyNameDimension = "Outcome dimension";
	const std::string m_propertyValueDimension1 = "1D";
	const std::string m_propertyValueDimension2 = "2D";
	const std::string m_propertyValueDimension3 = "3D";
	const std::string m_propertyValueDimensionCustom = "custom";

	const std::string m_propertyNameSeriesMetadata = "Measurement series";

	const std::string m_groupQuerySettings = "Query settings";
	const std::string m_groupQuantitySetttings = "Quantity settings";
	const std::string m_groupParamSettings1 = "Parameter 1";
	const std::string m_groupParamSettings2 = "Parameter 2";
	const std::string m_groupParamSettings3 = "Parameter 3";

	const std::string m_propertyName = "Name";
	const std::string m_propertyValueDescription = "Type";
	const std::string m_propertyDataType = "Data type";
	const std::string m_propertyComparator = "Comparator";
	const std::string m_propertyValue = "Value";
	const std::string m_propertyUnit = "Unit";

	ot::Connector m_connectorQuantity;
	ot::Connector m_connectorParameter1;
	ot::Connector m_connectorParameter2;
	ot::Connector m_connectorParameter3;

	void createUpdatedProperty(const std::string& _propName, const std::string& _propGroup, const std::string& _labelValue, EntityProperties& properties);

	const ValueComparisionDefinition getSelectedValueComparisionDefinition(const std::string& _groupName);
	bool setVisibleParameter2(bool _visible);
	bool setVisibleParameter3(bool _visible);
	bool setVisibleParameter(const std::string& _groupName,bool _visible);
	void updateBlockConfig();
	void updateConnections(std::list<std::string>& connectorsForRemoval);
	void removeConnectionsAtConnectedEntities(std::list<ot::UID>& connectionsForRemoval);

};
