// @otlicense
// File: IVisualisationPlot1D.h
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
#include "OTGui/Plot1DCfg.h"

class IVisualisationPlot1D
{
public:
	virtual const ot::Plot1DCfg getPlot() = 0;
	virtual void setPlot(const ot::Plot1DCfg& _config) = 0;
	virtual bool visualisePlot() = 0;
};
