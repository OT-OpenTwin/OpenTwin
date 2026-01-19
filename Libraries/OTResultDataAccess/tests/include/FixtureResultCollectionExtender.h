// @otlicense
// File: FixtureResultCollectionExtender.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
