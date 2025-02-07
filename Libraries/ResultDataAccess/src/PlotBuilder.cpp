#include "PlotBuilder.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityResult1DPlot_New.h"
#include "EntityResult1DCurve_New.h"
#include "OTModelAPI/ModelStateInformationHelper.h"
#include "MetadataSeries.h"
#include "AdvancedQueryBuilder.h"

PlotBuilder::PlotBuilder(ResultCollectionExtender& _extender, const std::string& _owner)
	:m_extender(_extender), m_owner(_owner)
{}

void PlotBuilder::addCurve(DatasetDescription&& _dataSetDescription, ot::Plot1DCurveInfoCfg& _config)
{
	const std::string labelY = _config.getLabelY();
	assert(labelY ==_dataSetDescription.getQuantityDescription()->getName());

	auto parameters = _dataSetDescription.getParameters();
	assert(parameters.size() == 1);
	std::shared_ptr<ParameterDescription> xAxis = *parameters.begin();
	
	const std::string labelX = _config.getLabelX();
	assert(labelY ==xAxis->getMetadataParameter().parameterName);

	storeCurve(std::move(_dataSetDescription), _config);

	ot::UID uid = EntityBase::getUidGenerator()->getUID();
	EntityResult1DCurve_New curveEntity(uid, nullptr, nullptr, nullptr, nullptr, m_owner);
	curveEntity.setName(_config.getName());
	curveEntity.createProperties();
	curveEntity.setCurve(_config);
	curveEntity.StoreToDataBase();

	ModelStateInformationHelper::addTopologyEntity(m_newModelStateInformation, curveEntity);
}

void PlotBuilder::buildPlot(const ot::Plot1DCfg& _plotCfg, bool _saveModelState)
{
	createPlot(_plotCfg);
	ot::ModelServiceAPI::addEntitiesToModel(m_newModelStateInformation, "Created new plot", _saveModelState);
}

void PlotBuilder::storeCurve(DatasetDescription&& _dataSetDescription, ot::Plot1DCurveInfoCfg& _config)
{ 
	//std::list<DatasetDescription> datasets{ std::move(_dataSetDescription) };
	//ot::UID seriesID = m_extender.buildSeriesMetadata(datasets, _config.getName(), {});
	//m_extender.processDataPoints(&(*datasets.begin()), seriesID);
	//
	//const std::string query = createQuery(seriesID);
	//_config.setQuery(query);

	//const std::string projection = createProjection();
	//_config.setProjection(projection);
}

const std::string PlotBuilder::createQuery(ot::UID _seriesID)
{
	const std::string query = "{" + MetadataSeries::getFieldName() + ":" + std::to_string(_seriesID) + "}";
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
	plotEntity.setName(_plotCfg.getName());
	plotEntity.createProperties();
	plotEntity.setPlot(_plotCfg);
	plotEntity.StoreToDataBase();

	ModelStateInformationHelper::addTopologyEntity(m_newModelStateInformation, plotEntity);
}
