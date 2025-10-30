// @otlicense
// File: PlotBuilder.cpp
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

#include "PlotBuilder.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityResult1DPlot.h"

#include "MetadataSeries.h"
#include "AdvancedQueryBuilder.h"
#include "OTGui/QueryInformation.h"
#include "OTCore/FolderNames.h"
#include "CurveFactory.h"
#include "QuantityDescriptionCurve.h"
#include "QuantityDescriptionCurveComplex.h"
#include "QuantityDescriptionMatrix.h"
#include "QuantityDescriptionSParameter.h"

PlotBuilder::PlotBuilder(ResultCollectionExtender& _extender)
	:m_extender(_extender)
{}

void PlotBuilder::addCurve(DatasetDescription&& _dataSetDescription, ot::Plot1DCurveCfg& _config, const std::string& _seriesName)
{
	std::list<DatasetDescription> datasets;
	datasets.push_back(std::move(_dataSetDescription));
	addCurve(std::move(datasets), _config, _seriesName);
}


void PlotBuilder::addCurve(std::list<DatasetDescription>&& _dataSetDescriptions, ot::Plot1DCurveCfg& _config, const std::string& _seriesName)
{
	const bool valid = validityCheck(_dataSetDescriptions, _config);
	assert(valid);
	if (!valid) {
		OT_LOG_E("Curve validity check failed!");
		return;
	}


	storeCurve(std::move(_dataSetDescriptions), _config, ot::FolderNames::DatasetFolder + "/" + _seriesName);

	ot::UID uid = EntityBase::getUidGenerator()->getUID();
	EntityResult1DCurve curveEntity(uid, nullptr, nullptr, nullptr, m_extender.getOwner());
	curveEntity.setName(_config.getEntityName());
	curveEntity.createProperties();

	m_quantityLabel.push_back(_config.getQueryInformation().m_quantityDescription.m_label);

	for (auto& parameterDescription : _config.getQueryInformation().m_parameterDescriptions)
	{
		m_parameterLabels.push_back(parameterDescription.m_label);
	}
	curveEntity.setCurve(_config);
	curveEntity.setEditable(true);
	m_curves.push_back(curveEntity);
}

void PlotBuilder::buildPlot(ot::Plot1DCfg& _plotCfg, bool _saveModelState)
{
	assert(!_plotCfg.getEntityName().empty());
	assert(m_curves.size() > 0);
	if (m_curves.size() <= 0) {
		OT_LOG_E("No curves exist to build plot!");
		return;
	}

	createPlot(_plotCfg);
	m_extender.setSaveModel(false);
	m_extender.storeCampaignChanges();
	ot::ModelServiceAPI::addEntitiesToModel(m_newModelStateInformation, "Created new plot",false, _saveModelState);
	clearBuffer();
}

bool PlotBuilder::validityCheck(std::list<DatasetDescription>& _dataSetDescriptions, ot::Plot1DCurveCfg& _config)
{
	//We need at least some data sets to create curves with
	if (_dataSetDescriptions.size() == 0)
	{
		return false;
	}
	
	//Next we check if every dataset has the necessary components
	bool valid = true;
	for (DatasetDescription& datasetDescription : _dataSetDescriptions)
	{	
		valid &= (datasetDescription.getParameters().size() != 0) && 
			(datasetDescription.getQuantityDescription() != nullptr) && 
			(datasetDescription.getQuantityDescription()->getMetadataQuantity().valueDescriptions.size() > 0 );
		QuantityDescription* quantityDescription = datasetDescription.getQuantityDescription();

		if (QuantityDescriptionCurve* curve = dynamic_cast<QuantityDescriptionCurve*>(quantityDescription)) {
			valid &= !curve->getDataPoints().empty();
		}
		else if (QuantityDescriptionCurveComplex* curveComplex = dynamic_cast<QuantityDescriptionCurveComplex*>(quantityDescription)) {
			valid &= (!curveComplex->getQuantityValuesImag().empty()) && (!curveComplex->getQuantityValuesReal().empty());
		}
		else if (QuantityDescriptionMatrix* matrix = dynamic_cast<QuantityDescriptionMatrix*>(quantityDescription)) {
			valid &= !matrix->getValues().empty();
		}
		else if (QuantityDescriptionSParameter* sparam = dynamic_cast<QuantityDescriptionSParameter*>(quantityDescription)) {
			valid &= (!sparam->getNumberOfFirstValues() == 0) && (!sparam->getNumberOfSecondValues() == 0);
		}
	}
	return valid;
}


void PlotBuilder::storeCurve(std::list<DatasetDescription>&& _dataSetDescriptions, ot::Plot1DCurveCfg& _config, const std::string& _seriesName)
{ 
	ot::UID seriesID = m_extender.buildSeriesMetadata(_dataSetDescriptions, _seriesName, {});
	for (auto& dataset : _dataSetDescriptions)
	{
		m_extender.processDataPoints(&dataset, seriesID);
	}
	
	const MetadataSeries* series =	m_extender.findMetadataSeries(seriesID);
	CurveFactory::addToConfig(*series, _config);
}

void PlotBuilder::clearBuffer()
{
	m_curves.clear();
	m_parameterLabels.clear();
	m_quantityLabel.clear();
}

void PlotBuilder::createPlot(ot::Plot1DCfg& _plotCfg)
{
	ot::UID uid = EntityBase::getUidGenerator()->getUID();

	EntityResult1DPlot plotEntity(uid, nullptr, nullptr, nullptr, m_extender.getOwner());
	const std::string entityName = _plotCfg.getEntityName();
	plotEntity.setName(entityName);

	if (_plotCfg.getTitle().empty())
	{
		const std::string shortName =  entityName.substr(entityName.find_last_of("/") + 1);
		_plotCfg.setTitle(shortName);
	}
	m_parameterLabels.sort();
	m_parameterLabels.unique();
	m_quantityLabel.sort();
	m_quantityLabel.unique();
	plotEntity.createProperties();
	plotEntity.setPlot(_plotCfg);
	plotEntity.setEditable(true);
	plotEntity.storeToDataBase();
	m_newModelStateInformation.addTopologyEntity(plotEntity);

	for (EntityResult1DCurve& curve : m_curves)
	{
		if (curve.getName().empty())
		{
			const std::string curveTitle = curve.getCurve().getTitle();
			const std::string curveEntityName =	entityName + "/" + curveTitle;
			curve.setName(curveEntityName);
		}

		curve.storeToDataBase();
		m_newModelStateInformation.addTopologyEntity(curve);
	}
}
