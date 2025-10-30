// @otlicense
// File: CurveFactory.h
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
#include "MetadataSeries.h"
#include "EntityResult1DCurve.h"
#include "OTGui/Plot1DCurveCfg.h"
class __declspec(dllexport) CurveFactory
{
public:
	static void addToConfig(const MetadataSeries& _series, ot::Plot1DCurveCfg& _config);

private:
	static const std::string createQuery(ot::UID _seriesID);
	static const std::string createProjection();
};
