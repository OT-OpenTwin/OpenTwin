#include "OTGui/VisualisationTypes.h"


void ot::VisualisationTypes::addTextVisualisation()
{
	m_visualisations.push_back(m_visualisationAsText);
}

void ot::VisualisationTypes::addTableVisualisation()
{
	m_visualisations.push_back(m_visualisationAsTable);
}

void ot::VisualisationTypes::addPlot1DVisualisation()
{
	m_visualisations.push_back(m_visualisationAsPlot1D);
}

void ot::VisualisationTypes::addCurveVisualisation()
{
	m_visualisations.push_back(m_visualisationAsCurve);
}

bool ot::VisualisationTypes::visualiseAsText()
{
	const auto entryIt = std::find(m_visualisations.begin(), m_visualisations.end(), m_visualisationAsText);
	return entryIt != m_visualisations.end();
}

bool ot::VisualisationTypes::visualiseAsTable()
{
	const auto entryIt = std::find(m_visualisations.begin(), m_visualisations.end(), m_visualisationAsTable);
	return entryIt != m_visualisations.end();
}

bool ot::VisualisationTypes::visualiseAsPlot1D()
{
	const auto entryIt = std::find(m_visualisations.begin(), m_visualisations.end(), m_visualisationAsPlot1D);
	return entryIt != m_visualisations.end();
}

bool ot::VisualisationTypes::visualiseAsCurve()
{
	const auto entryIt = std::find(m_visualisations.begin(), m_visualisations.end(), m_visualisationAsCurve);
	return entryIt != m_visualisations.end();
}

void ot::VisualisationTypes::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	std::list<uint32_t> tempList = m_visualisations;
	tempList.unique();
	ot::JsonArray visualisations;
	for (uint32_t visualisationIndx : tempList)
	{
		visualisations.PushBack(visualisationIndx,_allocator);
	}
	_object.AddMember("VisualisationTypes", visualisations, _allocator);

}

void ot::VisualisationTypes::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	auto visualisations = _object["VisualisationTypes"].GetArray();
	for (const auto& entry : visualisations)
	{
		uint32_t visualisationIndx = entry.GetUint();
		m_visualisations.push_back(visualisationIndx);
	}
}
