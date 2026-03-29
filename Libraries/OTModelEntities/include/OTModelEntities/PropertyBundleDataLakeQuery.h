#include "OTModelEntities/PropertyBundle.h"
#include "OTCore/MetadataHandle/MetadataCampaign.h"
#include "OTCore/Tuple/TupleInstance.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"

class __declspec(dllexport) PropertyBundleDataLakeQuery : public PropertyBundle
{
public:
	void setProperties(EntityBase* _entity) override;
	bool updatePropertyVisibility(EntityBase* _entity) override;

	std::string getSelectedProject(EntityBase* _entity);
	std::string getSelectedSeries(EntityBase* _entity);
	bool updateOptions(EntityBase* _entity, MetadataCampaign& _campaign);
	ot::ValueComparisonDescription getQuantityQuery (EntityBase* _entity) const;
	std::list<ot::ValueComparisonDescription> getParameterQueries(EntityBase* _entity) const;
	std::list<ot::ValueComparisonDescription> getMetadataQueries(EntityBase* _entity) const;

	std::list<std::string> getParameterOptions(const EntityBase* _thisObject) const;
	std::list<std::string> getQuantityOptions(const EntityBase* _thisObject) const;

private:
	const uint32_t m_maxNbOfQueries = 40;
	const uint32_t m_maxNbOfQueriesMetadata = 4;

	const std::string m_propertyNameProjectName = "Project name";
	const std::string m_propertyNameSeriesMetadata = "Measurement series";

	const std::string m_groupMetadataFilter = "Dataset";
	const std::string m_groupQuantitySettings = "Quantity settings";
	const std::string m_groupQuerySettings = "Query settings";
	const std::string m_groupTupleSettings = "Tuple settings";

	const std::string m_groupSeriesMetadata = "Series metadata";

	const std::string m_propertyNumberOfQueries = "Number of parameter queries";
	const std::string m_propertyNumberOfQueriesMetadataSeries = "Number of metadata queries";

	const std::string m_propertyName = "Name";
	const std::string m_propertyTupleFormat = "Format";
	const std::string m_propertyTupleTarget = "Target";
	const std::string m_propertyTupleUnit = "Units";
	const std::string m_propertyOrder = "Order reproducible";

	const std::string m_propertyDataType = "Data type";
	const std::string m_propertyComparator = "Comparator";
	const std::string m_propertyValue = "Value";
	const std::string m_propertyUnit = "Unit";

	void setNameOptions(EntityBase* _thisObject, const std::list<std::string>& _options, const std::string& _group, const std::string& _name);
	void setValueProperties(EntityBase* _thisObject, const std::string& _groupName, const ot::TupleInstance& _tupleInstance);
	void setValuePropertiesEmpty(EntityBase* _thisObject, const std::string& _groupName);

	void vectorize(const ot::JsonValue& _value, std::list<std::string>& _allEntries, const std::string& _nameBase);
};
