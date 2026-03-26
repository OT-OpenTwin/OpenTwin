#pragma once
//@otlicense
#include "OTCore/CoreAPIExport.h"
#include "OTCore/Serializable.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"

class OT_CORE_API_EXPORT DataLakeQueryCfg : public ot::Serializable
{
public:
	void setSeriesLabel(const std::string& _seriesLabel);
	void setCollectionName(const std::string& _collectionName);
	void setValueDescriptionParameters(const std::list<ot::ValueComparisonDescription>& _valueCompares);
	void setValueDescriptionQuantities(const ot::ValueComparisonDescription& _valueCompares);
	void setValueDescriptionSeriesMD(const std::list<ot::ValueComparisonDescription>& _valueCompares);

	const std::list<ot::ValueComparisonDescription>& getValueDescriptionParameters() const;
	const ot::ValueComparisonDescription& getValueDescriptionQuantities() const;
	const std::list<ot::ValueComparisonDescription>& getValueDescriptionSeriesMD() const;
	
	const std::string& getSeriesLabel() const { return m_seriesLabel; }
	const std::string& getCollectionName() const { return m_collectionName; }

	// Inherited via Serializable
	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
	void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;
private:
	std::string m_seriesLabel;
	std::string m_collectionName;
	std::list<ot::ValueComparisonDescription> m_valueDescriptionsParameters;
	ot::ValueComparisonDescription m_valueDescriptionsQuantities;
	std::list<ot::ValueComparisonDescription> m_valueDescriptionsSeriesMD;

};
