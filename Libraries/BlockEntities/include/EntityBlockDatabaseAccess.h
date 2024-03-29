#pragma once
#include "EntityBlock.h"
#include "ValueComparisionDefinition.h"

class __declspec(dllexport)  EntityBlockDatabaseAccess : public EntityBlock
{
public:
	EntityBlockDatabaseAccess(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockDatabaseAccess"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; }

	void createProperties(std::list<std::string>& comparators);
	std::string getSelectedProjectName();
	std::string getQueryDimension();

	const bool isQueryDimension1D();
	const bool isQueryDimension2D();
	const bool isQueryDimension3D();

	const std::string& getPropertyNameMeasurementSeries() { return _msmdPropertyName; }
	const std::string& getPropertyValueDimension1() { return _propertyValueDimension1; }
	const std::string& getPropertyValueDimension2() { return _propertyValueDimension2; }
	const std::string& getPropertyValueDimension3() { return _propertyValueDimension3; }

	const std::string& getPropertyNameQuantity() { return _propertyNameQuantity; }
	const std::string& getPropertyNameParameter1() { return _propertyNameP1; }
	const std::string& getPropertyNameParameter2() { return _propertyNameP2; }
	const std::string& getPropertyNameParameter3() { return _propertyNameP3; }

	const std::string& getPropertyDataTypeQuantity() { return _propertyDataTypeQuantity; }
	const std::string& getPropertyDataTypeParameter1() { return _propertyDataTypeP1; }
	const std::string& getPropertyDataTypeParameter2() { return _propertyDataTypeP2; }
	const std::string& getPropertyDataTypeParameter3() { return _propertyDataTypeP3; }

	const std::string& getGroupQuantity() { return _groupQuantitySetttings; }
	const std::string& getGroupParameter1() { return _groupParamSettings1; }
	const std::string& getGroupParameter2() { return _groupParamSettings2; }
	const std::string& getGroupParameter3() { return _groupParamSettings3; }


	const ValueComparisionDefinition getSelectedQuantityDefinition();
	const ValueComparisionDefinition getSelectedParameter1Definition();
	const ValueComparisionDefinition getSelectedParameter2Definition();
	const ValueComparisionDefinition getSelectedParameter3Definition();


	const ot::Connector getConnectorQuantity() const { return _connectorQuantity; }
	const ot::Connector getConnectorParameter1() const { return _connectorParameter1; }
	const ot::Connector getConnectorParameter2() const { return _connectorParameter2; }
	const ot::Connector getConnectorParameter3() const { return _connectorParameter3; }

	virtual bool updateFromProperties() override;

	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
protected:
	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	const std::string _propertyNameProjectName = "Projectname";

	const std::string _propertyNameDimension = "Outcome dimension";
	const std::string _propertyValueDimension1 = "1D";
	const std::string _propertyValueDimension2 = "2D";
	const std::string _propertyValueDimension3 = "3D";
	const std::string _propertyValueDimensionCustom = "custom";

	const std::string _msmdPropertyName = "Measurement series";

	const std::string _groupQuerySettings = "Query settings";

	const std::string _groupQuantitySetttings = "Quantity settings";
	const std::string _propertyNameQuantity = "Name";
	const std::string _propertyDataTypeQuantity = "Data type";
	const std::string _propertyComparator = "Comparator";
	const std::string _propertyValueQuantity = "Value";

	const std::string _groupParamSettings1 = "Parameter 1";
	const std::string _propertyNameP1 = "P1 name";
	const std::string _propertyDataTypeP1 = "P1 data type";
	const std::string _propertyComparatorP1 = "P1 comparator";
	const std::string _propertyValueP1 = "P1 query value";

	const std::string _groupParamSettings2 = "Parameter 2";
	const std::string _propertyNameP2 = "P2 name";
	const std::string _propertyDataTypeP2 = "P2 data type";
	const std::string _propertyComparatorP2 = "P2 comparator";
	const std::string _propertyValueP2 = "P2 query value";

	const std::string _groupParamSettings3 = "Parameter 3";
	const std::string _propertyNameP3 = "P3 name";
	const std::string _propertyDataTypeP3 = "P3 data type";
	const std::string _propertyComparatorP3 = "P3 comparator";
	const std::string _propertyValueP3 = "P3 query value";

	ot::Connector _connectorQuantity;
	ot::Connector _connectorParameter1;
	ot::Connector _connectorParameter2;
	ot::Connector _connectorParameter3;

	bool SetVisibleParameter2(bool visible);
	bool SetVisibleParameter3(bool visible);
	void UpdateBlockConfig();
	void UpdateConnections(std::list<std::string>& connectorsForRemoval);
	void RemoveConnectionsAtConnectedEntities(std::list<ot::UID>& connectionsForRemoval);
};
