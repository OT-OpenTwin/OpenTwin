// @otlicense
// File: StepWidthManager.h
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

class ObjectManager;

class Application;
class Properties;

class BoundingBox;
class EntityBase;

class TopoDS_Shape;
class TopoDS_Face;

#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>

namespace gmsh
{
	typedef std::vector<std::pair<int, int> > vectorpair;
}

class StepWidthManager
{
public:
	StepWidthManager(Application *app);
	~StepWidthManager() {};

	double getMaximumEdgeLength(void) { return maximumEdgeLength; }
	double getBoundingSphereMeshStep(void) { return boundingSphereMeshStep; }
	double getBackgroundBaseStep(void) { return backgroundBaseStep; }
	double getBackgroundDistance(void) { return backgroundDistance; }

	void determineBaseStepWidths(BoundingBox &boundingBox, Properties &properties);

	void buildIndexedFaceToStepSizeVector(EntityBase *entity, const TopoDS_Shape *shape, Properties &properties);
	void buildTagToAnnotationMap(gmsh::vectorpair &output);
	void applyVolumeMeshStepWidthToEntity(EntityBase *entity, ObjectManager &objectManager, Properties &properties);
	void applyCurvatureMeshStepWidthToEntities(void);
	void applyUserDefinedRefinmentsToFacesAndPoints(double refinementStep, std::string refinementList);

	void apply2DRefinementsToMesher(double refinementStep, double refinementRadius);
	void apply3DRefinementsToMesher(bool useDistanceForVolumeMeshRefinement, double refinementStep, double refinementRadius);

	bool hasAnyRefinementPoints(void) { return !refinementPoints.empty(); }

	void addProximityRefinementField(const std::string &expression, std::vector<double> &edgeList, std::vector<double> &faceList);
	void setProximityRefinementFields(void);

private:
	struct meshRefinement {
		double stepWidth;
		std::set<int> edgeSet;
		std::set<int> faceSet;
		std::set<int> volumeSet;
	};

	void getLocalMeshPropertiesForEntity(EntityBase *entity, bool &localCurvatureRefinement, int &localNumberOfStepsPerCircle, double &localMinCurvatureRefRadius, double &localMaximumDeviation);
	double getMaxStepWidthForFace(TopoDS_Face &face, bool localCurvatureRefinement, int localNumberOfStepsPerCircle, double localMinCurvatureRefRadius, double localMaximumDeviation);
	void applyVolumeMeshStepWidthToObjects(gmsh::vectorpair &shapeTags, double stepWidth);
	void applyMeshStepWidthToFaces(std::list<int> &refineFaceTagList, double stepWidth);
	std::string getRefinementFromString(std::string &refinementString);
	bool getPointFromRefinement(const std::string &refinement, double &x, double &y, double &z);
	bool getFaceIdFromRefinement(const std::string &refinement, int &faceId);
	void createRefinement(double stepWidth, std::set<int> &faceSet, std::set<int> &edgeSet, std::set<int> &volumeSet);
	void convertSet(const std::set<int> &setData, std::vector<double> &listData);

	Application *application;

	double maximumEdgeLength;
	double boundingSphereMeshStep;
	double backgroundBaseStep;
	double backgroundDistance;
	int    backgroundMeshRefinementField;

	std::vector<double>                   faceStepWidthVector;
	std::map<int, double>                 faceTagToStepSizeMap;
	std::map<std::string, meshRefinement> meshFields;
	std::list<std::array<double, 3>>      refinementPoints;
	std::vector<double>					  refinementFields2D;

};