// @otlicense

#include <gtest/gtest.h>	
#include "FixtureResultCollectionExtender.h"
#include "EntityContainer.h"
#include "DataBase.h"

TEST_F(FixtureResultCollectionExtender, AddingParameter_Same)
{
	MetadataParameter expectedFrequencyParameter = getFrequencyParameter();
	expectedFrequencyParameter.parameterUID = 0;
	expectedFrequencyParameter.parameterLabel = "";
	auto frequencyParameterDescr(std::make_shared<ParameterDescription>(expectedFrequencyParameter, true));

	DatasetDescription newDataset;
	newDataset.addParameterDescription(frequencyParameterDescr);

	ot::UIDList parameterIDs =	addCampaignContextDataToParameters(newDataset);
	MetadataParameter actualFrequencyParameter = frequencyParameterDescr->getMetadataParameter();
	EXPECT_EQ(expectedFrequencyParameter.parameterUID, actualFrequencyParameter.parameterUID);
	EXPECT_EQ(expectedFrequencyParameter.parameterUID, *parameterIDs.begin());
}

TEST_F(FixtureResultCollectionExtender, AddingParameter_AddTwiceToCampaign)
{
	MetadataParameter differentParameter;
	differentParameter.parameterName = "Cheese";
	differentParameter.typeName = ot::TypeNames::getStringTypeName();
	differentParameter.unit = "Type";
	differentParameter.values = { "Chedder", "Amsterdammer", "Gorgonzola" };

	auto parameterDescr(std::make_shared<ParameterDescription>(differentParameter, true));

	DatasetDescription newDataset;
	newDataset.addParameterDescription(parameterDescr);

	ot::UIDList parameterIDs = addCampaignContextDataToParameters(newDataset);
	updateCampaignOverview(); //Action that is performed after the creation of a series. Updates overview of already existing metadata in the campaign
	parameterIDs = addCampaignContextDataToParameters(newDataset);

	auto allParameter =	newDataset.getParameters();
	auto firstFrequencyParameter =	(*allParameter.begin())->getMetadataParameter();
	auto secondFrequencyParameter =	(*allParameter.begin()++)->getMetadataParameter();

	EXPECT_EQ(firstFrequencyParameter.parameterUID, secondFrequencyParameter.parameterUID);
	EXPECT_EQ(firstFrequencyParameter.parameterLabel, secondFrequencyParameter.parameterLabel);
}

TEST_F(FixtureResultCollectionExtender, AddingParameter_AddTwiceInSameSeries)
{
	MetadataParameter differentParameter;
	differentParameter.parameterName = "Cheese";
	differentParameter.typeName = ot::TypeNames::getStringTypeName();
	differentParameter.unit = "Type";
	differentParameter.values = { "Chedder", "Amsterdammer", "Gorgonzola" };

	auto parameterDescr(std::make_shared<ParameterDescription>(differentParameter, true));

	DatasetDescription newDataset;
	newDataset.addParameterDescription(parameterDescr);

	ot::UIDList parameterIDs = addCampaignContextDataToParameters(newDataset); //Here no update campaign overview. This simulates two datasets being used for the creation of a new series.
	parameterIDs = addCampaignContextDataToParameters(newDataset);

	auto allParameter = newDataset.getParameters();
	auto firstFrequencyParameter = (*allParameter.begin())->getMetadataParameter();
	auto secondFrequencyParameter = (*allParameter.begin()++)->getMetadataParameter();

	EXPECT_EQ(firstFrequencyParameter.parameterUID, secondFrequencyParameter.parameterUID);
	EXPECT_EQ(firstFrequencyParameter.parameterLabel, secondFrequencyParameter.parameterLabel);
}

TEST_F(FixtureResultCollectionExtender, AddingParameter_EqualTwiceInSameSeries)
{
	MetadataParameter differentParameter;
	differentParameter.parameterName = "Cheese";
	differentParameter.typeName = ot::TypeNames::getStringTypeName();
	differentParameter.unit = "Type";
	differentParameter.values = { "Chedder", "Amsterdammer", "Gorgonzola" };

	auto parameterDescr(std::make_shared<ParameterDescription>(differentParameter, false));
	auto parameterDescr2(std::make_shared<ParameterDescription>(differentParameter, false));
	DatasetDescription newDataset;
	newDataset.addParameterDescription(parameterDescr);
	newDataset.addParameterDescription(parameterDescr2);

	ot::UIDList parameterIDs = addCampaignContextDataToParameters(newDataset);

	auto allParameter = newDataset.getParameters();
	auto firstParameter = (*allParameter.begin())->getMetadataParameter();
	auto secondParameter = (*allParameter.begin()++)->getMetadataParameter();
	EXPECT_EQ(firstParameter.parameterUID, secondParameter.parameterUID);
	EXPECT_EQ(firstParameter.parameterUID, secondParameter.parameterUID);
}

TEST_F(FixtureResultCollectionExtender, AddingParameter_DifferentParameterSameName)
{
	MetadataParameter differentFrequencyParameter;
	differentFrequencyParameter.parameterName = "Frequency";
	differentFrequencyParameter.typeName = ot::TypeNames::getDoubleTypeName();
	differentFrequencyParameter.unit = "kHz";

	MetadataParameter existingFrequencyParameter = getFrequencyParameter();

	ASSERT(differentFrequencyParameter != existingFrequencyParameter);
	ASSERT(differentFrequencyParameter.parameterName == existingFrequencyParameter.parameterName);
	
	DatasetDescription newDataset;
	newDataset.addParameterDescription(std::make_shared<ParameterDescription>(differentFrequencyParameter,true));

	ot::UIDList parameterIDs = addCampaignContextDataToParameters(newDataset);
	auto newFrequencyParameter = (*newDataset.getParameters().begin())->getMetadataParameter();
	EXPECT_NE(newFrequencyParameter.parameterUID, existingFrequencyParameter.parameterUID);
	EXPECT_NE(newFrequencyParameter.parameterLabel, existingFrequencyParameter.parameterLabel);

}


//TEST_F(FixtureResultCollectionExtender, AddingQuantity_Same)
//{
//	MetadataQuantity quantityNew = getPowerQuantity();
//	quantityNew.quantityIndex = 0;
//	quantityNew.quantityLabel = "";
//	auto frequencyParameterDescr(std::make_shared<QuantityDescription>(quantityNew, true));
//
//	DatasetDescription newDataset;
//	newDataset.addParameterDescription(frequencyParameterDescr);
//
//	ot::UIDList parameterIDs = addCampaignContextDataToParameters(newDataset);
//	MetadataParameter actualFrequencyParameter = frequencyParameterDescr->getMetadataParameter();
//	EXPECT_EQ(expectedFrequencyParameter.parameterUID, actualFrequencyParameter.parameterUID);
//	EXPECT_EQ(expectedFrequencyParameter.parameterUID, *parameterIDs.begin());
//}
//
//TEST_F(FixtureResultCollectionExtender, AddingQuantity_SameTwiceInCampaign)
//{
//
//}
//
//TEST_F(FixtureResultCollectionExtender, AddingQuantity_SameTwiceInSameSeries)
//{
//
//}
//
//TEST_F(FixtureResultCollectionExtender, AddingQuantity_EqualTwiceInSameSeries)
//{
//
//}
//
//TEST_F(FixtureResultCollectionExtender, AddingQuantity_DifferentParameterSameName)
//{
//
//}