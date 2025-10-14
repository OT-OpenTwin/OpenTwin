//! @file VisualisationTypes.h
//! @author Jan Wagner, Alexander Kuester (alexk95)
//! @date September 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Serializable.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/WidgetViewBase.h"

// std header
#include <map>
#include <list>
#include <optional>

#pragma warning(disable : 4251)

namespace ot
{
	class OT_GUI_API_EXPORT VisualisationTypes : public ot::Serializable {
	public:
		enum VisualisationType {
			None               = 0 << 0,
			Text               = 1 << 0,
			Table              = 1 << 1,
			Plot1D             = 1 << 2,
			Curve              = 1 << 3,
			Range              = 1 << 4,
			GraphicsView       = 1 << 5,
			GraphicsItem       = 1 << 6,
			GraphicsConnection = 1 << 7
		};
		typedef Flags<VisualisationType> VisualisationTypeFlags;

		VisualisationTypes() = default;
		~VisualisationTypes() = default;

		void setVisualisations(const VisualisationTypeFlags& _types) { m_visualisations = _types; };
		void addVisualisation(VisualisationType _type) { m_visualisations |= _type; };

		void addTextVisualisation() { m_visualisations |= Text; };
		void addTableVisualisation() { m_visualisations |= Table; };
		void addPlot1DVisualisation() { m_visualisations |= Plot1D; };
		void addCurveVisualisation() { m_visualisations |= Curve; };
		void addRangeVisualisation() { m_visualisations |= Range; };
		void addGraphicsViewVisualisation() { m_visualisations |= GraphicsView; };
		void addGraphicsItemVisualisation() { m_visualisations |= GraphicsItem; };
		void addGraphicsConnectionVisualisation() { m_visualisations |= GraphicsConnection; };

		const VisualisationTypeFlags& getVisualisations() const { return m_visualisations; };
		bool visualiseAsText() const { return m_visualisations & Text; };
		bool visualiseAsTable() const { return m_visualisations & Table; };
		bool visualiseAsPlot1D() const { return m_visualisations & Plot1D; };
		bool visualiseAsCurve() const { return m_visualisations & Curve; };
		bool visualiseAsRange() const { return m_visualisations & Range; };
		bool visualiseAsGraphicsView() const { return m_visualisations & GraphicsView; };
		bool visualiseAsGraphicsItem() const { return m_visualisations & GraphicsItem; };
		bool visualiseAsGraphicsConnection() const { return m_visualisations & GraphicsConnection; };

		//! @brief Sets custom view flags for a specific visualisation type.
		//! @param _visType The visualisation type for which the custom flags should be set.
		//! @param _flags The custom view flags to be applied.
		void setCustomViewFlags(VisualisationType _visType, ot::WidgetViewBase::ViewFlags _flags) { m_customViewFlags.insert_or_assign(_visType, _flags); };

		//! @brief Retrieves custom view flags for a specific visualisation type, if they exist.
		//! @param _visType The visualisation type for which to retrieve custom flags.
		//! @return An optional containing the custom view flags if they exist, or std::nullopt if not.
		std::optional<ot::WidgetViewBase::ViewFlags> getCustomViewFlags(VisualisationType _visType) const;

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	private:
		VisualisationTypeFlags m_visualisations;
		std::map<VisualisationType, ot::WidgetViewBase::ViewFlags> m_customViewFlags;
	};
}

OT_ADD_FLAG_FUNCTIONS(ot::VisualisationTypes::VisualisationType)