// @otlicense
// File: ModelBuilder.h
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

class Properties;
class StepWidthManager;
class MaterialManager;
class Application;

class EntityGeometry;
class EntityBase;
class EntityAnnotation;

class BRepAlgoAPI_BuilderAlgo;

#include <vector>
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <utility>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt2d.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include "OTGui/Properties/PropertyGridCfg.h"

class ModelBuilder
{
public:
	ModelBuilder(Application *app) : application(app) , storeGeometry(true) {};
	~ModelBuilder();

	void setStoreGeometry(bool flag) { storeGeometry = flag; };

	void buildModel(const std::string &meshName, std::list<EntityGeometry *> &geometryEntities, Properties &properties, StepWidthManager &stepWidthManager, MaterialManager &materialManager);

	std::list<EntityGeometry *> &getModelEntities(void) { return modelEntities; }
	std::list<std::vector<std::string>>  &getAllShapeOverlaps(void) { return allShapeOverlaps; }

	struct ShapeDiagnosticPoint
	{
		gp_Pnt point;
		TopoDS_Shape sourceShape;
		std::string reason;
	};

	struct CoincidentFacePair
	{
		TopoDS_Face first;
		TopoDS_Face second;
		double distance = 0.0;
	};

	struct ThinGapFacePair
	{
		TopoDS_Face first;
		TopoDS_Face second;

		double minDistance = 0.0;
		double meanDistance = 0.0;
		double maxDistance = 0.0;
		double hitRatio = 0.0;

		std::vector<gp_Pnt> diagnosticPoints;
	};

	struct ShapeCheckReport
	{
		bool occValid = false;
		bool hasSolid = false;
		bool hasPositiveVolume = false;
		bool closed = true;
		bool manifold = true;
		bool meshable = false;

		double totalVolume = 0.0;

		std::vector<TopoDS_Shape> invalidSubShapes;

		std::vector<TopoDS_Edge> freeEdges;
		std::vector<TopoDS_Edge> nonManifoldEdges;
		std::vector<TopoDS_Edge> isolatedEdges;
		std::vector<TopoDS_Edge> degeneratedEdges;
		std::vector<TopoDS_Edge> tooShortEdges;

		std::vector<TopoDS_Face> invalidFaces;
		std::vector<TopoDS_Face> tooSmallFaces;

		std::vector<TopoDS_Solid> invalidSolids;
		std::vector<TopoDS_Solid> negativeOrZeroVolumeSolids;

		std::vector<CoincidentFacePair> coincidentFaces;
		std::vector<ThinGapFacePair> thinGapFaces;

		std::vector<ShapeDiagnosticPoint> diagnosticPoints;

		std::vector<std::string> messages;

		std::string toString() const;
	};

	ShapeCheckReport checkShapeForVolumeMeshing(
		const TopoDS_Shape& shape,
		double minEdgeLength = 1.0e-9,
		double minFaceArea = 1.0e-18,
		double minVolume = 1.0e-27,
		double gapTolerance = 0.1,
		bool checkGeometry = true) const;

private:
	std::string checkMaterialAssignmentForShapes(std::list<EntityGeometry *> &geometryEntities);
	std::string checkMaterialAssignmentForBoundingSphere(Properties &properties);
	std::string addBoundingSphere(std::list<EntityGeometry *> &geometryEntities, Properties &properties, StepWidthManager &stepWidthManager);
	void buildNonManifoldModel(const std::string &meshName, std::list<EntityGeometry *> &geometryEntities, Properties &properties, MaterialManager &materialManager, StepWidthManager& stepWidthManager);
	double getMeshPriority(EntityBase *entity, MaterialManager &materialManager);
	void createAllShapes(BRepAlgoAPI_BuilderAlgo &booleanOperation, const std::string &meshName, EntityGeometry *entity, double meshPriority);
	void analyzeOverlaps(void);
	void resolveOverlaps(void);
	void removeUnnecessaryHierarchies(void);
	void mergeShapesOfSameParent(void);
	bool mergeChildren(std::vector<std::string> &childList, std::map<std::string, EntityGeometry *> &shapesInEntityList);
	EntityGeometry *createGeometryEntity(const std::string &name, TopoDS_Shape &shape, const ot::PropertyGridCfg& shapeProperties);
	void storeEntities(void);

	void collectOccInvalidSubShapes(
		const TopoDS_Shape& shape,
		ShapeCheckReport& report,
		bool checkGeometry) const;

	void checkEdgeIncidence(
		const TopoDS_Shape& shape,
		ShapeCheckReport& report) const;

	void checkDegeneratedAndSmallEdges(
		const TopoDS_Shape& shape,
		ShapeCheckReport& report,
		double minEdgeLength) const;

	void checkSmallFaces(
		const TopoDS_Shape& shape,
		ShapeCheckReport& report,
		double minFaceArea) const;

	void checkSolidsAndVolumes(
		const TopoDS_Shape& shape,
		ShapeCheckReport& report,
		double minVolume) const;

	bool areFacesCoincidentBySampling(
		const TopoDS_Face& a,
		const TopoDS_Face& b,
		double distanceTolerance,
		int samplesPerDirection,
		int minInteriorHits) const;

	void checkCoincidentFacesInList(
		const std::vector<TopoDS_Face>& faces,
		ShapeCheckReport& report,
		double distanceTolerance,
		double areaTolerance,
		int samplesPerDirection,
		int minInteriorHits) const;

	void checkCoincidentFaces(
		const TopoDS_Shape& shape,
		ShapeCheckReport& report,
		double distanceTolerance,
		double areaTolerance,
		int samplesPerDirection,
		int minInteriorHits) const;

	bool samplePointOnFace(
		const TopoDS_Face& face,
		double u,
		double v,
		gp_Pnt& point) const;

	bool isPointStrictlyInsideFaceUV(
		const TopoDS_Face& face,
		const gp_Pnt& point,
		double tolerance) const;

	bool isPointStrictlyInsideFace(
		const TopoDS_Face& face,
		const gp_Pnt& point,
		double tolerance) const;

	bool isPlanarFace(const TopoDS_Face& face) const;

	bool isToleratedTouchingEdge(
		const TopoDS_Edge& edge,
		const TopTools_ListOfShape& adjacentFaces,
		double angleTolerance = 1.0e-3) const;

	bool computeFaceNormalNearEdge(
		const TopoDS_Face& face,
		const TopoDS_Edge& edge,
		gp_Vec& normal) const;

	void extractFaceTriangles(
		const TopoDS_Face& face,
		EntityAnnotation* annotation,
		double r,
		double g,
		double b,
		double linearDeflection = 1.0e-3,
		double angularDeflection = 0.5) const;

	void checkThinGaps(
		const TopoDS_Shape& shape,
		ShapeCheckReport& report,
		double gapTolerance,
		double areaTolerance,
		int samplesPerDirection = 9,
		double minHitRatio = 0.30,
		double maxRelativeStdDev = 0.5) const;

	void checkThinGapsInList(
		const std::vector<TopoDS_Face>& faces,
		ShapeCheckReport& report,
		double gapTolerance,
		double areaTolerance,
		int samplesPerDirection,
		double minHitRatio,
		double maxDistanceVariation) const;

	bool analyzeThinGapBySampling(
		const TopoDS_Face& a,
		const TopoDS_Face& b,
		ThinGapFacePair& result,
		double gapTolerance,
		int samplesPerDirection,
		double minHitRatio,
		double maxDistanceVariation) const;

	bool computeFaceNormalAtUV(
		const TopoDS_Face& face,
		double u,
		double v,
		gp_Vec& normal) const;

	void visualizeThinGapFaces(
		const ShapeCheckReport& report,
		EntityAnnotation* annotation,
		double linearDeflection = 1.0e-3,
		double angularDeflection = 0.5) const;

	double edgeLength(const TopoDS_Edge& edge) const;
	double faceArea(const TopoDS_Face& face) const;
	double solidVolume(const TopoDS_Solid& solid) const;
	gp_Pnt computeEdgeMidPoint(const TopoDS_Edge& edge) const;
	gp_Pnt computeFaceCenterPoint(const TopoDS_Face& face) const;
	void checkShapesForMeshing(const std::string& meshName, double gapTolerance);

	void collectDiagnosticPoints(ShapeCheckReport& report) const;

	std::string brepCheckStatusToString(int status) const;
	Application *application;
	std::list<EntityGeometry *> modelEntities;
	std::map<std::string, double> meshPriorities;
	std::map<std::string, std::string> parentShapeName;
	std::list<std::vector<std::string>> allShapeOverlaps;
	bool storeGeometry;
};
