#include "PlotBuilder.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityResult1DPlot_New.h"

#include "OTModelAPI/ModelStateInformationHelper.h"
#include "MetadataSeries.h"
#include "AdvancedQueryBuilder.h"
#include "OTGui/QueryInformation.h"
#include "OTCore/FolderNames.h"

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
	
	storeCurve(std::move(_dataSetDescriptions), _config, ot::FolderNames::DatasetFolder + "/" + _seriesName);

	ot::UID uid = EntityBase::getUidGenerator()->getUID();
	EntityResult1DCurve_New curveEntity(uid, nullptr, nullptr, nullptr, nullptr, m_extender.getOwner());
	curveEntity.setName(_config.getEntityName());
	curveEntity.createProperties();

	m_quantityLabel.push_back(_config.getQueryInformation().m_quantityDescription.m_label);

	for (auto& parameterDescription : _config.getQueryInformation().m_parameterDescriptions)
	{
		m_parameterLabels.push_back(parameterDescription.m_label);
	}
	curveEntity.setCurve(_config);
	m_curves.push_back(curveEntity);
}

void PlotBuilder::buildPlot(ot::Plot1DCfg& _plotCfg, bool _saveModelState)
{
	assert(!_plotCfg.getEntityName().empty());
	assert(m_curves.size() > 0);

	createPlot(_plotCfg);
	m_extender.setSaveModel(false);
	m_extender.storeCampaignChanges();
	ot::ModelServiceAPI::addEntitiesToModel(m_newModelStateInformation, "Created new plot", _saveModelState);
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
	}
	
	if (!valid)
	{
		return valid;
	}

	//Only for now with a single y-axis
	const std::string labelY = _config.getYAxisTitle();

	for (auto& datasetDescription : _dataSetDescriptions)
	{
		valid &= (labelY == datasetDescription.getQuantityDescription()->getName());		
		//The x label is flexible, multiple options may exist, thus a comparision does not make sense
	}

	return valid;
}

void PlotBuilder::setDefaults(std::list<DatasetDescription>& _dataSetDescriptions, ot::Plot1DCurveCfg& _config)
{
	for (auto& datasetDescription : _dataSetDescriptions)
	{
		if (datasetDescription.getParameters().size() == 1)
		{
			const MetadataParameter& parameter = (*datasetDescription.getParameters().begin())->getMetadataParameter();
			if (_config.getXAxisTitle().empty())
			{
				_config.setXAxisTitle(parameter.parameterLabel);
			}
			if (_config.getXAxisUnit().empty())
			{
				_config.setXAxisUnit(parameter.unit);
			}
		}
	}
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
	quantityInformation.m_label = quantities.begin()->quantityName;
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

void PlotBuilder::clearBuffer()
{
	m_curves.clear();
	m_parameterLabels.clear();
	m_quantityLabel.clear();
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

void PlotBuilder::createPlot(ot::Plot1DCfg& _plotCfg)
{
	ot::UID uid = EntityBase::getUidGenerator()->getUID();

	EntityResult1DPlot_New plotEntity(uid, nullptr, nullptr, nullptr, nullptr, m_extender.getOwner());
	const std::string entityName = _plotCfg.getEntityName();
	plotEntity.setName(entityName);

	if (_plotCfg.getTitle().empty())
	{
		const std::string shortName =  entityName.substr(entityName.find_last_of("/") + 1);
		_plotCfg.setTitle(shortName);
	}
	m_parameterLabels.unique();
	m_quantityLabel.unique();
	plotEntity.setFamilyOfCurveProperties(m_parameterLabels,m_quantityLabel);
	plotEntity.createProperties();
	plotEntity.setPlot(_plotCfg);
	plotEntity.StoreToDataBase();


	for (EntityResult1DCurve_New& curve : m_curves)
	{
		if (curve.getName().empty())
		{
			const std::string curveTitle = curve.getCurve().getTitle();
			const std::string curveEntityName =	entityName + "/" + curveTitle;
			curve.setName(curveEntityName);
		}

		curve.StoreToDataBase();
		ModelStateInformationHelper::addTopologyEntity(m_newModelStateInformation, curve);
	}
	
	m_newModelStateInformation.m_topologyEntityIDs.insert(m_newModelStateInformation.m_topologyEntityIDs.begin(), plotEntity.getEntityID());
	m_newModelStateInformation.m_topologyEntityVersions.insert(m_newModelStateInformation.m_topologyEntityVersions.begin(), plotEntity.getEntityStorageVersion());
	m_newModelStateInformation.m_forceVisible.push_back(false);
}
