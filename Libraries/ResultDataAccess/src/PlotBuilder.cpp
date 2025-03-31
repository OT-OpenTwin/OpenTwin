#include "PlotBuilder.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityResult1DPlot_New.h"
#include "EntityResult1DCurve_New.h"
#include "OTModelAPI/ModelStateInformationHelper.h"
#include "MetadataSeries.h"
#include "AdvancedQueryBuilder.h"
#include "OTGui/QueryInformation.h"

PlotBuilder::PlotBuilder(ResultCollectionExtender& _extender, const std::string& _owner)
	:m_extender(_extender), m_owner(_owner)
{}

void PlotBuilder::addCurve(DatasetDescription&& _dataSetDescription, ot::Plot1DCurveCfg& _config, const std::string& _seriesName)
{
	std::list<DatasetDescription> datasets;
	datasets.push_back(std::move(_dataSetDescription));
 	addCurveFamily(std::move(datasets), _config, _seriesName);
}


void PlotBuilder::addCurveFamily(std::list<DatasetDescription>&& _dataSetDescriptions, ot::Plot1DCurveCfg& _config, const std::string& _seriesName)
{
	const bool valid = validityCheck(_dataSetDescriptions, _config);
	assert(valid);
	
	storeCurve(std::move(_dataSetDescriptions), _config, _seriesName);

	ot::UID uid = EntityBase::getUidGenerator()->getUID();
	EntityResult1DCurve_New curveEntity(uid, nullptr, nullptr, nullptr, nullptr, m_owner);
	curveEntity.setName(_config.getEntityName());
	curveEntity.createProperties();
	curveEntity.setCurve(_config);
	curveEntity.StoreToDataBase();

	ModelStateInformationHelper::addTopologyEntity(m_newModelStateInformation, curveEntity);
}

void PlotBuilder::buildPlot(const ot::Plot1DCfg& _plotCfg, bool _saveModelState)
{
	createPlot(_plotCfg);
	m_extender.setSaveModel(false);
	m_extender.storeCampaignChanges();
	ot::ModelServiceAPI::addEntitiesToModel(m_newModelStateInformation, "Created new plot", _saveModelState);
}

bool PlotBuilder::validityCheck(std::list<DatasetDescription>& _dataSetDescriptions, ot::Plot1DCurveCfg& _config)
{
	bool valid = true;
	const std::string labelY = _config.getYAxisTitle();
	
	for (auto& datasetDescription : _dataSetDescriptions)
	{
		valid &= (labelY == datasetDescription.getQuantityDescription()->getName());
		auto parameters = datasetDescription.getParameters();

		std::shared_ptr<ParameterDescription> xAxis = *parameters.begin();
		const std::string labelX = _config.getXAxisTitle();
		valid &= (labelX == xAxis->getMetadataParameter().parameterName);
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
	
	ot::QueryInformation queryInformation;
	queryInformation.m_query= createQuery(seriesID);
	queryInformation.m_projection = createProjection();
	
	const MetadataSeries* series =	m_extender.findMetadataSeries(seriesID);
	const std::list<MetadataQuantity>& quantities = series->getQuantities();
	assert(quantities.size() == 1);
	if (quantities.size() != 1)
	{
		throw std::invalid_argument("Creating a curve is only possible with a single quantity");
	}
	auto quantity =	quantities.begin()->valueDescriptions.begin();
	
	ot::QuantityContainerEntryDescription quantityInformation;
	quantityInformation.m_fieldName = QuantityContainer::getFieldName();
	quantityInformation.m_unit = quantity->unit;
	quantityInformation.m_dataType = quantity->dataTypeName;
	queryInformation.m_quantityDescription = quantityInformation;

	const std::list<MetadataParameter>& parameters = series->getParameter();

	for (const auto& parameter : parameters)
	{	
		ot::QuantityContainerEntryDescription qcDescription;
		qcDescription.m_label =  parameter.parameterLabel;
		qcDescription.m_dataType=  parameter.typeName;
		qcDescription.m_fieldName=  std::to_string(parameter.parameterUID);
		qcDescription.m_unit =  parameter.unit;
		queryInformation.m_parameterDescriptions.push_back(qcDescription);
	}

	_config.setQueryInformation(queryInformation);
}

const std::string PlotBuilder::createQuery(ot::UID _seriesID)
{
	const std::string query = "{\"" + MetadataSeries::getFieldName() + "\":" + std::to_string(_seriesID) + "}";
	return query;
}

const std::string PlotBuilder::createProjection()
{
	AdvancedQueryBuilder builder;
	std::vector<std::string> projectionNamesForExclusion{ "SchemaVersion", "SchemaType", MetadataSeries::getFieldName()};
	const std::string projection = bsoncxx::to_json(builder.GenerateSelectQuery(projectionNamesForExclusion, false, false));
	return projection;
}

void PlotBuilder::createPlot(const ot::Plot1DCfg& _plotCfg)
{
	ot::UID uid = EntityBase::getUidGenerator()->getUID();

	EntityResult1DPlot_New plotEntity(uid, nullptr, nullptr, nullptr, nullptr, m_owner);
	plotEntity.setName(_plotCfg.getEntityName());
	plotEntity.createProperties();
	plotEntity.setPlot(_plotCfg);
	plotEntity.StoreToDataBase();

	m_newModelStateInformation.m_topologyEntityIDs.insert(m_newModelStateInformation.m_topologyEntityIDs.begin(), plotEntity.getEntityID());
	m_newModelStateInformation.m_topologyEntityVersions.insert(m_newModelStateInformation.m_topologyEntityVersions.begin(), plotEntity.getEntityStorageVersion());
	m_newModelStateInformation.m_forceVisible.push_back(false);
}
