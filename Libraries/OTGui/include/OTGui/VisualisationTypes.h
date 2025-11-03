// @otlicense
// File: VisualisationTypes.h
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
			GraphicsConnection = 1 << 7,
			Image              = 1 << 8,
			PDF                = 1 << 9
		};
		typedef Flags<VisualisationType> VisTypes;

		VisualisationTypes() = default;
		~VisualisationTypes() = default;

		void setVisualisations(const VisTypes& _types) { m_visualisations = _types; };
		void addVisualisation(VisualisationType _type) { m_visualisations |= _type; };

		void addTextVisualisation() { m_visualisations |= Text; };
		void addTableVisualisation() { m_visualisations |= Table; };
		void addPlot1DVisualisation() { m_visualisations |= Plot1D; };
		void addCurveVisualisation() { m_visualisations |= Curve; };
		void addRangeVisualisation() { m_visualisations |= Range; };
		void addGraphicsViewVisualisation() { m_visualisations |= GraphicsView; };
		void addGraphicsItemVisualisation() { m_visualisations |= GraphicsItem; };
		void addGraphicsConnectionVisualisation() { m_visualisations |= GraphicsConnection; };
		void addImageVisualisation() { m_visualisations |= Image; };
		void addPDFVisualisation() { m_visualisations |= PDF; };

		bool hasVisualisations() const { return m_visualisations != VisualisationType::None; };
		bool hasVisualisation(VisualisationType _type) const { return m_visualisations & _type; };
		const VisTypes& getVisualisations() const { return m_visualisations; };
		bool visualiseAsText() const { return m_visualisations & Text; };
		bool visualiseAsTable() const { return m_visualisations & Table; };
		bool visualiseAsPlot1D() const { return m_visualisations & Plot1D; };
		bool visualiseAsCurve() const { return m_visualisations & Curve; };
		bool visualiseAsRange() const { return m_visualisations & Range; };
		bool visualiseAsGraphicsView() const { return m_visualisations & GraphicsView; };
		bool visualiseAsGraphicsItem() const { return m_visualisations & GraphicsItem; };
		bool visualiseAsGraphicsConnection() const { return m_visualisations & GraphicsConnection; };
		bool visualiseAsImage() const { return m_visualisations & Image; };
		bool visualiseAsPDF() const { return m_visualisations & PDF; };

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
		VisTypes m_visualisations;
		std::map<VisualisationType, ot::WidgetViewBase::ViewFlags> m_customViewFlags;
	};
}

OT_ADD_FLAG_FUNCTIONS(ot::VisualisationTypes::VisualisationType, ot::VisualisationTypes::VisTypes)