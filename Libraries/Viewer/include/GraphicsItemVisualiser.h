// @otlicense
// File: GraphicsItemVisualiser.h
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

#include "Visualiser.h"

class GraphicsItemVisualiser : public Visualiser {
	OT_DECL_NOCOPY(GraphicsItemVisualiser)
	OT_DECL_NOMOVE(GraphicsItemVisualiser)
	OT_DECL_NODEFAULT(GraphicsItemVisualiser)
public:
	GraphicsItemVisualiser(SceneNodeBase* _sceneNode);
	bool requestVisualization(const VisualiserState& _state, ot::VisualiserInfo& _info) override;
	virtual void showVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) override;
	virtual void hideVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) override;

	virtual std::string getViewEntityName() const override;

protected:
	virtual std::string getVisualiserTypeString() const override { return "GraphicsConnection"; };

	SceneNodeBase* findViewNode(SceneNodeBase* _childNode) const;

};
