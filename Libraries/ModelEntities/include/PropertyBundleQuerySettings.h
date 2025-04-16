#pragma once
#include "PropertyBundle.h"


class __declspec(dllexport) PropertyBundleQuerySettings : public PropertyBundle
{
public:
	virtual void setProperties(EntityBase* _thisObject) override;
	virtual bool updatePropertyVisibility(EntityBase* _thisObject) override;

private:
	const std::string m_groupQuerySettings = "Query settings";
	const std::string m_propertyNbOfComparisions = "Number of queries";

	const std::string m_groupQueryDefinition = "Query definition";
	const std::string m_propertyName = "Name";
	const std::string m_propertyComparator = "Comparator";
	const std::string m_propertyValue = "Value";

	const uint32_t m_maxNumberOfQueryDefinitions = 10;
};
