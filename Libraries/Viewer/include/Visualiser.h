// @otlicense
// File: Visualiser.h
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
#include "OTCore/CoreTypes.h"
#include "OTGui/WidgetViewBase.h"
#include "VisualiserState.h"
#include "OTGui/VisualisationCfg.h"

// std header
#include <optional>

class SceneNodeBase;

class Visualiser {
	OT_DECL_NOCOPY(Visualiser)
	OT_DECL_NOMOVE(Visualiser)
	OT_DECL_NODEFAULT(Visualiser)
public:
	Visualiser(SceneNodeBase* _sceneNode, ot::WidgetViewBase::ViewType _viewType);
	virtual ~Visualiser();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Visualization handling

	//! @brief Request a visualization for the entity.
	//! This is called if the entity is currently not visualized.
	//! @param _state The state of the visualisation.
	//! @return True if a new view was requested, false otherwise.
	virtual bool requestVisualization(const VisualiserState& _state) = 0;

	//! @brief Request the next data chunk for the visualisation.
	//! @param _nextChunkStartIndex The index where the next data chunk should start.
	//! @return True if a new data chunk was requested, false otherwise.
	virtual bool requestNextDataChunk(size_t _nextChunkStartIndex) { return false; };

	//! @brief Request the remaining data for the visualisation.
	//! @param _nextChunkStartIndex The index where the next data chunk should start.
	//! @return True if the remaining data was requested, false otherwise.
	virtual bool requestRemainingData(size_t _nextChunkStartIndex) { return false; };

	//! @brief Show or undim the visualization for thentity.
	//! This is called if the entity has already been visualized before.
	virtual void showVisualisation(const VisualiserState& _state) = 0;

	//! @brief Hide or dim the visualisation.
	//! @note The view close handling is performed by the view manager.
	//! The visualiser should only hide or dim the visualisation.
	virtual void hideVisualisation(const VisualiserState& _state) = 0;

	bool mayVisualise() const { return m_mayVisualise; };

	//! @brief Switch to turn a visualisation type off
	void setMayVisualise(bool _visible) { m_mayVisualise = _visible;}

	//! @brief Switch for deciding if a data pull is necessary or if a lookup is necessary, if the displayed data may still be up-to date
	virtual void setViewIsOpen(bool _viewIsOpen) { m_viewIsOpen = _viewIsOpen; };
	bool getViewIsOpen() const { return m_viewIsOpen; };

	void setCloseViewOnDelete(bool _closeOnDelete) { m_closeViewOnDelete = _closeOnDelete; };
	bool getCloseViewOnDelete() const { return m_closeViewOnDelete; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Information

	SceneNodeBase* getSceneNode() const { return m_node; };
	
	//! @brief Set the entity that is visualized by this visualiser.
	//! @param _entity The entity ID of the entity that is visualized by this visualiser.
	void setVisualizationEntity(ot::UID _entity) { m_visualizationEntity = _entity; };
	ot::UID getVisualizationEntity() const { return m_visualizationEntity; };

	void setCustomViewFlags(const ot::WidgetViewBase::ViewFlags& _flags) { m_customViewFlags = _flags; };
	const std::optional<ot::WidgetViewBase::ViewFlags>& getCustomViewFlags() const { return m_customViewFlags; };

	ot::WidgetViewBase::ViewType getViewType() const { return m_viewType; };

	virtual void getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const;

	//! @brief Create a visualiser configuration for this visualiser.
	//! @param _state The state of the visualisation.
	//! @param _rootNode Optional root node for the visualisation. If provided only this node and its children will be considered for the visualizing entities.
	ot::VisualisationCfg createVisualiserConfig(const VisualiserState& _state, SceneNodeBase* _rootNode = nullptr) const;

protected:
	virtual std::string getVisualiserTypeString() const = 0;

	ot::UID m_visualizationEntity;
	SceneNodeBase* m_node = nullptr;
	bool m_mayVisualise = true;
	bool m_viewIsOpen = false;
	bool m_closeViewOnDelete = true;
	ot::WidgetViewBase::ViewType m_viewType;
	std::optional<ot::WidgetViewBase::ViewFlags> m_customViewFlags = std::nullopt;
};
