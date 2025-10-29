// @otlicense

#include "FixtureResultCollectionExtender.h"

#include "MetadataEntrySingle.h"
#include "MetadataEntryArray.h"

void FixtureResultCollectionExtender::initiateCampaignWithOneSeries()
{
	MetadataSeries seriesMetadata("seriesOne");
	MetadataParameter parameter = m_parameterFrequency;
	seriesMetadata.addParameter(parameter);
	MetadataParameter parameter2 = m_parameterRadius;
	seriesMetadata.addParameter(parameter2);

	MetadataQuantity quantity = m_quantityPower;
	seriesMetadata.addQuantity(quantity);
	m_extender.m_metadataCampaign.addSeriesMetadata(std::move(seriesMetadata));
	updateCampaignOverview();
}

FixtureResultCollectionExtender::FixtureResultCollectionExtender()
{
	m_parameterFrequency = createFrequency();
	m_quantityPower = createPower();
	m_parameterRadius = createRadius();
	initiateCampaignWithOneSeries();
}

ot::UIDList FixtureResultCollectionExtender::addCampaignContextDataToParameters(DatasetDescription& _dataDescription)
{
	return m_extender.addCampaignContextDataToParameters(_dataDescription);
}

void FixtureResultCollectionExtender::updateCampaignOverview()
{
	m_extender.m_metadataCampaign.updateMetadataOverviewFromLastAddedSeries();
}

MetadataParameter FixtureResultCollectionExtender::createFrequency()
{
	MetadataParameter parameter;
	parameter.parameterName = "Frequency";
	parameter.parameterLabel = "Frequency";
	parameter.parameterUID = 1;
	parameter.unit = "Hz";
	parameter.values = { 1.0,1.5,2.0,2.5 };
	parameter.typeName = ot::TypeNames::getDoubleTypeName();
	parameter.metaData["Meta"] = std::make_shared<MetadataEntrySingle>("Meta",ot::Variable(13));
	return parameter;
}

MetadataQuantity FixtureResultCollectionExtender::createPower()
{
	MetadataQuantity quantity;
	quantity.quantityName = "Power";
	quantity.quantityLabel = "Power";
	quantity.quantityIndex = 1;
	quantity.dataDimensions = { 1 };
	quantity.dependingParameterIds = { 1 };
	quantity.metaData["Meta"] = std::make_shared<MetadataEntrySingle>("Meta", ot::Variable(13));

	MetadataQuantityValueDescription realValue;
	realValue.quantityIndex = 1;
	realValue.quantityValueName = "real";
	realValue.quantityValueLabel = "real";
	realValue.unit = "W";
	realValue.dataTypeName = ot::TypeNames::getDoubleTypeName();
	quantity.valueDescriptions.push_back(realValue);

	MetadataQuantityValueDescription imagValue;
	imagValue.quantityIndex = 1;
	imagValue.quantityValueName = "imag";
	imagValue.quantityValueLabel = "imag";
	imagValue.unit = "W";
	imagValue.dataTypeName = ot::TypeNames::getDoubleTypeName();
	quantity.valueDescriptions.push_back(imagValue);

	return quantity;
}

MetadataParameter FixtureResultCollectionExtender::createRadius()
{
	MetadataParameter parameter;
	parameter.parameterName = "Radius";
	parameter.parameterLabel = "Radius";
	parameter.parameterUID = 2;
	parameter.unit = "cm";
	parameter.values = { 4.0 };
	parameter.typeName = ot::TypeNames::getDoubleTypeName();
	parameter.metaData["Meta"] = std::make_shared<MetadataEntrySingle>("Meta", ot::Variable(13));
	return parameter;
}
