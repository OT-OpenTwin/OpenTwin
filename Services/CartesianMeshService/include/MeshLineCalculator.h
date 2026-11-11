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

#include<array>
#include<list>
#include<vector>

#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/BoundingBox.h"
#include "OTModelEntities/EntityMeshCartesian.h"
#include "OTModelEntities/Geometry.h"

#include <TopoDS_Shape.hxx>

class ProblemType;

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

struct MeshLineCalculatorEdgeRefinement
{
	BoundingBox boundingBox;
	std::array<double, 3> stepWidth = { 0.0, 0.0, 0.0 }; // X/Y/Z; non-positive values disable that direction
	int parallelAxis = -1; // -1: general edge; 0/1/2: straight edge parallel to X/Y/Z
};

class MeshLineCalculator
{
public:
	MeshLineCalculator() : maximumEdgeLength(0.0), stepsAlongDiagonal(0.0), maximumMeshRatio(0.0), meshEqulibrationRatio(0.0), meshEntity(nullptr), problemType(nullptr) {};
	~MeshLineCalculator() {};

	void setMesh(EntityMeshCartesian* mesh) { meshEntity = mesh; }
	void setMeshEntities(const std::list<EntityBase*>& entities) { meshEntities = entities; }
	void setMaximumEdgeLength(double value) { maximumEdgeLength = value; }
	void setStepsAlongDiagonal(double value) { stepsAlongDiagonal = value; }
	void setMaximumMeshRatio(double value) { maximumMeshRatio = value; }
	void setMeshEqulibrationRatio(double value) { meshEqulibrationRatio = value; }
	void setProblemType(ProblemType* _problemType) { problemType = _problemType; }

	void setEdgeRefinements(const std::list<MeshLineCalculatorEdgeRefinement>& refinements) { edgeRefinements = refinements; }

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
	double getVolumeMeshStepWidth(int direction, EntityBase* entity, double baseStepWidth);
	std::vector<double> createDensityBasedGrid(std::vector<double> fixedLines, const std::vector<MeshLineCalculatorStepRange>& ranges);
	double findPositionForDensity(double a, double b, double targetDensity, const std::vector<MeshLineCalculatorStepRange>& ranges);
	double densityIntegral(double a, double b, const std::vector<MeshLineCalculatorStepRange>& ranges);
	std::vector<double> determineMeshLines(const std::list<MeshLineCalculatorWeightedPoint>& fixPlanes, const std::vector<MeshLineCalculatorStepRange>& densityRanges);
	bool intervalViolatesStep(double x0, double x1, const std::vector<MeshLineCalculatorStepRange>& ranges);
	void refineUntilValid(std::vector<double>& grid, const std::vector<MeshLineCalculatorStepRange>& ranges);
	std::vector<double> equilibrateMeshLines(std::vector<double> meshLines, double meshEquilibrationRatio);
	std::vector<double> splitGrowing(double length, double neighborStep, double ratio);
	void determineEdgeRefinments(double baseStepWidth);
	void appendEdgeRefinementsFromBrep(const TopoDS_Shape& brep, double edgeStepWidthX, double edgeStepWidthY, double edgeStepWidthZ);

	// Data
	EntityMeshCartesian* meshEntity;

	double maximumEdgeLength;
	double stepsAlongDiagonal;
	double maximumMeshRatio;
	double meshEqulibrationRatio;

	double angleToleranceDeg = 0.1;
	double geometryTolerance = 1e-5;

	std::list<MeshLineCalculatorEdgeRefinement> edgeRefinements;
	std::list<EntityBase*> meshEntities;
	ProblemType* problemType;

	std::vector<double> meshCoords[3];
};
