// @otlicense
// File: ProximityMeshing.h
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

class Application;
class SelfIntersectionCheck;
class ProgressLogger;
class StepWidthManager;

class EntityAnnotation;

#include <list>
#include <vector>
#include <tuple>
#include <string>

class ProximityMeshing
{
public:
	ProximityMeshing(Application *app);
	~ProximityMeshing() {};

	bool fixMeshIntersections(const std::string &meshName, int proximityMeshing, ProgressLogger *logger, StepWidthManager *stepWidthManager, bool verbose);
	void storeErrorAnnotations(void);

private:
	void loadSelfIntersectionObject(SelfIntersectionCheck &intersectionChecker);
	bool checkSelfIntersection(SelfIntersectionCheck &checker);
	void visualiseSelfIntersections(const std::string &meshName, std::vector < std::tuple<double, double, double, uint32_t, uint32_t> > IntersectingInfo, std::list<EntityAnnotation *> &errorAnnotation);
	std::string quadraticExpression(double localSize, double Sdelta, std::vector<double> partnerFaces);
	std::string linearExpression(double localSize, double Sdelta, std::vector<double> partnerFaces);

	Application *application;
	std::list<EntityAnnotation *> errorAnnotations;
};
