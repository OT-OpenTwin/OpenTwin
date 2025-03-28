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
	const std::string labelY = _config.getYAxisTitle();
	assert(labelY ==_dataSetDescription.getQuantityDescription()->getName());

	auto parameters = _dataSetDescription.getParameters();
	assert(parameters.size() == 1);
	std::shared_ptr<ParameterDescription> xAxis = *parameters.begin();
	
	const std::string labelX = _config.getXAxisTitle();
	assert(labelX ==xAxis->getMetadataParameter().parameterName);

	storeCurve(std::move(_dataSetDescription), _config,_seriesName);

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

void PlotBuilder::storeCurve(DatasetDescription&& _dataSetDescription, ot::Plot1DCurveCfg& _config, const std::string& _seriesName)
{ 
	std::list<DatasetDescription> datasets;
	datasets.push_back(std::move(_dataSetDescription));
	ot::UID seriesID = m_extender.buildSeriesMetadata(datasets, _seriesName, {});
	m_extender.processDataPoints(&(*datasets.begin()), seriesID);
	
	ot::QueryInformation queryInformation;
	queryInformation.m_query= createQuery(seriesID);
	queryInformation.m_projection = createProjection();
	
	const MetadataSeries* series =	m_extender.findMetadataSeries(seriesID);
	const std::list<MetadataQuantity>& quantities = series->getQuantities();
	assert(quantities.size() == 1);
	auto quantity =	quantities.begin()->valueDescriptions.begin();
	
	ot::QuantityContainerEntryDescription quantityInformation;
	quantityInformation.m_fieldName = QuantityContainer::getFieldName();
	quantityInformation.m_unit = quantity->unit;
	quantityInformation.m_dataType = quantity->dataTypeName;
	queryInformation.m_quantityDescription = quantityInformation;

	auto dataSetDescription = datasets.begin();
	const auto& parameterDescriptions = dataSetDescription->getParameters();
	for (auto parameterDescription : parameterDescriptions)
	{
		MetadataParameter& parameter =	parameterDescription->getMetadataParameter();
		
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
