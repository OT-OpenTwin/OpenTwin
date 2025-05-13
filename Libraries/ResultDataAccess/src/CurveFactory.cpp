#include "CurveFactory.h"
#include "QuantityContainer.h"
#include "OTGui/QuantityContainerEntryDescription.h"
#include "AdvancedQueryBuilder.h"

void CurveFactory::addToConfig(const MetadataSeries& _series, ot::Plot1DCurveCfg& _config)
{
	ot::QueryInformation queryInformation;
	queryInformation.m_query = createQuery(_series.getSeriesIndex());
	queryInformation.m_projection = createProjection();

	const std::list<MetadataQuantity>& quantities = _series.getQuantities();
	assert(quantities.size() == 1);
	if (quantities.size() != 1)
	{
		throw std::invalid_argument("Creating a curve is only possible with a single quantity");
	}
	auto quantity = quantities.begin()->valueDescriptions.begin();

	ot::QuantityContainerEntryDescription quantityInformation;
	quantityInformation.m_fieldName = QuantityContainer::getFieldName();
	quantityInformation.m_label = quantities.begin()->quantityName;
	quantityInformation.m_unit = quantity->unit;
	quantityInformation.m_dataType = quantity->dataTypeName;
	queryInformation.m_quantityDescription = quantityInformation;

	const std::list<MetadataParameter>& parameters = _series.getParameter();

	for (const auto& parameter : parameters)
	{
		ot::QuantityContainerEntryDescription qcDescription;
		qcDescription.m_label = parameter.parameterLabel;
		qcDescription.m_dataType = parameter.typeName;
		qcDescription.m_fieldName = std::to_string(parameter.parameterUID);
		qcDescription.m_unit = parameter.unit;
		queryInformation.m_parameterDescriptions.push_back(qcDescription);
	}

	_config.setQueryInformation(queryInformation);
}

const std::string CurveFactory::createQuery(ot::UID _seriesID)
{
	const std::string query = "{\"" + MetadataSeries::getFieldName() + "\":" + std::to_string(_seriesID) + "}";
	return query;
}

const std::string CurveFactory::createProjection()
{
	AdvancedQueryBuilder builder;
	std::vector<std::string> projectionNamesForExclusion{ "SchemaVersion", "SchemaType", MetadataSeries::getFieldName() };
	const std::string projection = bsoncxx::to_json(builder.GenerateSelectQuery(projectionNamesForExclusion, false, false));
	return projection;
}
