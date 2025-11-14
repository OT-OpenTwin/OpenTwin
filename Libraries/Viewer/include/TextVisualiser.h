// @otlicense
// File: TextVisualiser.h
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
#include "Visualiser.h"
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/VisualisationCfg.h"

// std header
#include <string>

class TextVisualiser : public Visualiser {
public:

	TextVisualiser(SceneNodeBase* _sceneNode);
	
	//! @brief Visualisation logic for different types of visualisation. Mostly requests to the model service which actually has the needed data.
	//! @return true, if a new visualisation was requested
	virtual bool requestVisualization(const VisualiserState& _state) override;
	virtual bool requestNextDataChunk(size_t _nextChunkStartIndex) override;
	virtual void showVisualisation(const VisualiserState& _state) override;
	virtual void hideVisualisation(const VisualiserState& _state) override;

protected:
	virtual std::string getVisualiserTypeString() const override { return "Text"; };

private:
	ot::JsonDocument createRequestDoc(const VisualiserState& _state, size_t _nextChunkStartIndex) const;

};
