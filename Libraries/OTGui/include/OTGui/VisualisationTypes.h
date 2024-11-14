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

		bool visualiseAsText();
		bool visualiseAsTable();

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;
	private:

		std::list<uint32_t> m_visualisations;
		const uint32_t m_visualisationAsText = 0;
		const uint32_t m_visualisationAsTable = 0;
	};
}
