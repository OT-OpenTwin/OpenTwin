// @otlicense
// File: MeshLineCalculator.h
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

#include<list>

#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/BoundingBox.h"
#include "OTModelEntities/EntityMeshCartesian.h"
#include "OTModelEntities/Geometry.h"

struct MeshLineCalculatorWeightedPoint
{
	double coord;
	double area;
	double priority;
};

struct MeshLineCalculatorStepRange
{
	double min;
	double max;
	double step;
};

class MeshLineCalculator
{
public:
	MeshLineCalculator() : maximumEdgeLength(0.0), stepsAlongDiagonal(0.0), meshEntity(nullptr) {};
	~MeshLineCalculator() {};

	void setMesh(EntityMeshCartesian* mesh) { meshEntity = mesh; }
	void setMeshEntities(const std::list<EntityBase*>& entities) { meshEntities = entities; }
	void setMaximumEdgeLength(double value) { maximumEdgeLength = value; }
	void setStepsAlongDiagonal(double value) { stepsAlongDiagonal = value; }

	void updateMeshLines();

	std::vector<double>& getMeshLines(int direction) { return meshCoords[direction]; }

private:
	BoundingBox calculateBoundingBox();
	BoundingBox determineBoundingBoxExtension(const BoundingBox &geometryBoundingBox);
	EntityMeshCartesian* getEntityMesh() { assert(meshEntity != nullptr); return meshEntity; }
	std::list<MeshLineCalculatorWeightedPoint> determineFixPlanes(int direction, double vx, double vy, double vz, double min, double max);
	void calculateTriangleNormal(Geometry::Node& n1, Geometry::Node& n2, Geometry::Node& n3, double& nx, double& ny, double& nz);
	bool isParallelOrAntiparallel(double nx, double ny, double nz, double vx, double vy, double vz, double deltaAngleDeg);
	double calculateTriangleArea(Geometry::Node& n1, Geometry::Node& n2, Geometry::Node& n3);
	std::list<MeshLineCalculatorWeightedPoint> mergeValuesAndAveragePositions(const std::list<MeshLineCalculatorWeightedPoint>& input, double tolerance);
	std::list<MeshLineCalculatorWeightedPoint> mergeWeightedPoints(std::list<MeshLineCalculatorWeightedPoint> input, double tolerance);
	std::vector<MeshLineCalculatorStepRange> determineDensityRanges(int direction, double boundingBoxMin, double boundingBoxMax, double baseStepWidth);
	void addRange(std::vector<MeshLineCalculatorStepRange>& ranges, double newMin, double newMax, double newStep);
	double getVolumeMeshStepWidth(EntityBase* entity, double baseStepWidth);
	double getStepAt(double x, const std::vector<MeshLineCalculatorStepRange>& ranges);
	double getNextRangeBoundary(double x, double xEnd, const std::vector<MeshLineCalculatorStepRange>& ranges);
	std::vector<double> refineGridMarching(std::vector<double> baseGrid, const std::vector<MeshLineCalculatorStepRange>& ranges);
	std::vector<double> determineMeshLines(const std::list<MeshLineCalculatorWeightedPoint>& fixPlanes, const std::vector<MeshLineCalculatorStepRange>& densityRanges);
	std::vector<double> equilibrateMeshLines(std::vector<double> meshLines, double maximumMeshRatio);
	std::vector<double> splitGrowing(double length, double neighborStep, double ratio);

	// Data
	EntityMeshCartesian* meshEntity;

	double maximumEdgeLength;
	double stepsAlongDiagonal;
	double smallestCellRatio = 0.1;
	double angleToleranceDeg = 1.0;
	double geometryTolerance = 1e-5;

	std::list<EntityBase*> meshEntities;

	std::vector<double> meshCoords[3];
};
