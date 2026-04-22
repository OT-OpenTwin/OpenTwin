// @otlicense
// File: EntityCableHarness.h
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

#include "OTCADEntities/EntitySmartPart.h"

#include <map>
#include <list>
#include <vector>

class OT_CADENTITIES_API_EXPORT EntityCableHarness : public EntitySmartPart
{
public:
	EntityCableHarness() : EntityCableHarness(0, nullptr, nullptr, nullptr) {};
	EntityCableHarness(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);
	virtual ~EntityCableHarness();

	static std::string className() { return "EntityCableHarness"; };
	virtual std::string getClassName(void) const override { return EntityCableHarness::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual void updatePart(void) override;

protected:
	virtual void createPartProperties(void) override;

private:
	virtual int getSchemaVersion(void) override { return 1; };
	void createHarnessFacets(std::map<std::string, std::tuple<double, double, double>>& harnessNodes, std::set<std::tuple<std::string, std::string>>& harnessSegments, std::vector<Geometry::Node>& nodes, std::list<Geometry::Triangle>& triangles, std::list<Geometry::Edge>& edges, double tubeRadius, int nTubeSegments);
	void extractHarnessData(const std::string& harnessData, std::map<std::string, std::tuple<double, double, double>>& harnessNodes, std::set<std::tuple<std::string, std::string>>& harnessSegments);
	void addCapsule(const double p1[3], const double p2[3], double tubeRadius, int nTubeSegments, ot::UID faceId, std::list<Geometry::Node>& nodes, std::list<Geometry::Triangle>& triangles);

};


