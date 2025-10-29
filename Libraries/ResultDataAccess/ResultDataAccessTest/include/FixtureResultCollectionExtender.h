// @otlicense

#pragma once
#include <gtest/gtest.h>

#include "ResultCollectionExtenderMockUp.h"
class FixtureResultCollectionExtender : public testing::Test
{
public:
	FixtureResultCollectionExtender();
	const MetadataParameter& getFrequencyParameter() const { return m_parameterFrequency; }
	const MetadataParameter& getRadiusParameter() const { return m_parameterRadius; }
	const MetadataQuantity& getPowerQuantity()const { return m_quantityPower; }
	ot::UIDList addCampaignContextDataToParameters(DatasetDescription& _dataDescription);

	ResultCollectionExtenderMockUp& getExtender() { return m_extender; }
	void updateCampaignOverview();
private:
	ResultCollectionExtenderMockUp m_extender;
	MetadataParameter m_parameterFrequency;
	MetadataParameter m_parameterRadius;
	MetadataQuantity m_quantityPower;
	MetadataParameter createFrequency();
	MetadataQuantity createPower();
	MetadataParameter createRadius();
	void initiateCampaignWithOneSeries();
};
