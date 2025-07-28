#pragma once
#include "OTGuiAPIExport.h"
#include "OTCore/Serializable.h"
#include "OTCore/JSON.h"
#include <list>
#include <stdint.h>

#pragma warning(disable : 4251)

namespace ot
{
	class OT_GUI_API_EXPORT VisualisationTypes : public ot::Serializable
	{
	public:
		VisualisationTypes() = default;
		~VisualisationTypes() = default;

		void addTextVisualisation();
		void addTableVisualisation();
		void addPlot1DVisualisation();
		void addCurveVisualisation();
		void addRangeVisualisation();

		bool visualiseAsText() const;
		bool visualiseAsTable() const;
		bool visualiseAsPlot1D() const;
		bool visualiseAsCurve() const;
		bool visualiseAsRange() const;

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;
	private:

		std::list<uint32_t> m_visualisations;
		const uint32_t m_visualisationAsText = 0;
		const uint32_t m_visualisationAsTable = 1;
		const uint32_t m_visualisationAsPlot1D = 2;
		const uint32_t m_visualisationAsCurve = 3;
		const uint32_t m_visualisationAsRange = 4;
	};
}
