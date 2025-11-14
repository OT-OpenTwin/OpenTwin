// @otlicense
// File: VisualisationCfg.h
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
#include "OTGui/WidgetViewBase.h"

// std header
#include <optional>

namespace ot
{
	class OT_GUI_API_EXPORT VisualisationCfg : public ot::Serializable
	{
		OT_DECL_DEFCOPY(VisualisationCfg)
		OT_DECL_DEFMOVE(VisualisationCfg)
	public:
		VisualisationCfg(const std::string& _visualisationType, const bool _setAsActiveView, const ot::UIDList& _visualisingEntities, const bool _overrideViewerContent, const bool _supressViewHandling)
			: m_visualisationType(_visualisationType), m_setAsActiveView(_setAsActiveView), m_visualisingEntities(_visualisingEntities), m_overrideViewerContent(_overrideViewerContent), m_supressViewHandling(_supressViewHandling)
		{}

		VisualisationCfg() = default;
		~VisualisationCfg() = default;

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setAsActiveView(bool _setAsActiveView) { m_setAsActiveView = _setAsActiveView; };
		bool getSetAsActiveView() const { return m_setAsActiveView; };

		void setVisualisingEntities(const ot::UIDList& _visualisingEntities) { m_visualisingEntities = _visualisingEntities; };
		void addVisualisingEntity(ot::UID _visualisingEntity) { m_visualisingEntities.push_back(_visualisingEntity); };
		const ot::UIDList& getVisualisingEntities() const { return m_visualisingEntities; };

		void setOverrideViewerContent(bool _overrideViewerContent) { m_overrideViewerContent = _overrideViewerContent; };
		bool getOverrideViewerContent() const { return m_overrideViewerContent; };

		void setLoadNextChunkOnly(bool _loadNextChunkOnly) { m_loadNextChunkOnly = _loadNextChunkOnly; };
		bool getLoadNextChunkOnly() const { return m_loadNextChunkOnly; };

		void setNextChunkStartIndex(size_t _nextChunkStartIndex) { m_nextChunkStartIndex = _nextChunkStartIndex; };
		size_t getNextChunkStartIndex() const { return m_nextChunkStartIndex; };

		void setSupressViewHandling(bool _supressViewHandling) { m_supressViewHandling = _supressViewHandling; };
		bool getSupressViewHandling() const { return m_supressViewHandling; };
		
		void setVisualisationType(const std::string _visualisationType) { m_visualisationType = _visualisationType; };
		const std::string getVisualisationType() const { return m_visualisationType; };

		void setCustomViewFlags(const ot::WidgetViewBase::ViewFlags& _flags) { m_customViewFlags = _flags; };
		const std::optional<ot::WidgetViewBase::ViewFlags>& getCustomViewFlags() const { return m_customViewFlags; };

	private:
		bool m_setAsActiveView = true;
		ot::UIDList m_visualisingEntities;
		bool m_overrideViewerContent = true;
		bool m_loadNextChunkOnly = false;
		size_t m_nextChunkStartIndex = 0;
		bool m_supressViewHandling = false;
		std::string m_visualisationType = "";
		std::optional<ot::WidgetViewBase::ViewFlags> m_customViewFlags = std::nullopt;
	};
}

