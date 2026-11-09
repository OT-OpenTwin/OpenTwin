// @otlicense
// File: SequenceDiaSceneBuilder.h
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
#include "OTGui/Diagram/SequenceDiaFunctionCfg.h"
#include "OTWidgets/WidgetTypes.h"

// std header
#include <map>

namespace ot {

	class GraphicsView;
	class SequenceDiaLifeLine;

	class OT_WIDGETS_API_EXPORT SequenceDiaSceneBuilder
	{
		OT_DECL_NOCOPY(SequenceDiaSceneBuilder)
		OT_DECL_NOMOVE(SequenceDiaSceneBuilder)
		OT_DECL_NODEFAULT(SequenceDiaSceneBuilder)
	public:
		
		SequenceDiaSceneBuilder(GraphicsView* _graphicsView);
		virtual ~SequenceDiaSceneBuilder();

		void build();

		void addFunction(const SequenceDiaFunctionCfg& _functionCfg);

	private:
		bool m_needsBuild = true;
		GraphicsView* m_view = nullptr;
		std::list<SequenceDiaLifeLine*> m_lifeLines;

	};

}