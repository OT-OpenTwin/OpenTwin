// @otlicense
// File: GraphicsVisualiser.h
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

class GraphicsVisualiser : public Visualiser {
	OT_DECL_NOCOPY(GraphicsVisualiser)
	OT_DECL_NOMOVE(GraphicsVisualiser)
	OT_DECL_NODEFAULT(GraphicsVisualiser)
public:
	GraphicsVisualiser(SceneNodeBase* _sceneNode);
	virtual bool requestVisualization(const VisualiserState& _state, ot::VisualiserInfo& _info) override;
	virtual void showVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) override;
	virtual void hideVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) override;
	virtual void setViewIsOpen(bool _viewIsOpen) override;

protected:
	virtual std::string getVisualiserTypeString() const override { return "Graphics"; };

private:
	void forwardViewOpenToChildren(bool _isOpen, SceneNodeBase* _parentNode);
};
