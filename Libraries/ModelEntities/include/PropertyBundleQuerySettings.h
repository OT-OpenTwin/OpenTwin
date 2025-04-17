#pragma once
#include "PropertyBundle.h"
#include "OTCore/ValueComparisionDefinition.h"

class __declspec(dllexport) PropertyBundleQuerySettings : public PropertyBundle
{
public:
	virtual void setProperties(EntityBase* _thisObject) override;
	virtual bool updatePropertyVisibility(EntityBase* _thisObject) override;
	void setQueryDefinitions(const std::list<std::string>& _queryOptions);
	void reload(EntityBase* _thisObject);

	std::list<ValueComparisionDefinition> getValueComparisionDefinitions(EntityBase* _thisObject);

private:
	const std::string m_groupQuerySettings = "Query settings";
	const std::string m_propertyNbOfComparisions = "Number of queries";

	const std::string m_groupQueryDefinition = "Query definition";
	const std::string m_propertyName = "Name";
	const std::string m_propertyComparator = "Comparator";
	const std::string m_propertyValue = "Value";

	std::string m_quantityName = "";
	uint32_t m_maxNumberOfQueryDefinitions = 0;
	std::list<std::string> m_selectionOptions;
};
