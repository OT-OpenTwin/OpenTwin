// @otlicense
// File: Properties.cpp
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

#include "Properties.h"

#include "EntityMeshTet.h"

#define _USE_MATH_DEFINES

#include <math.h>

Properties::Properties() :
	checkMaterial(false),
	boundingSphereAbsolute(false),
	meshAlg2D(-1),
	meshAlg3D(-1),
	elementOrder(1),
	maximumEdgeLengthValue(0.0),
	meshOptimization(false),
	proximityMeshing(-1),
	curvatureRefinement(false),
	numberOfStepsPerCircle(-1),
	minCurvatureRefRadius(0.0),
	usePriorities(false),
	mergeShapes(false),
	refinementStep(0.0),
	refinementRadius(0.0),
	boundingSphereRadius(0.0),
	stepsAlongDiagonal(0.0),
	maximumDeviation(0.0),
	verbose(false),
	meshStepsOnBoundingSphere(0.0),
	tolerance(0.0),
	useDistanceForVolumeMeshRefinement(false),
	boundingSphereMaterial(nullptr)
{

}

void Properties::readSettings(EntityMeshTet *mesh)
{
	EntityPropertiesDouble *maximumEdgeLengthProperty = dynamic_cast<EntityPropertiesDouble*>(mesh->getProperties().getProperty("Maximum edge length"));
	assert(maximumEdgeLengthProperty != nullptr);

	EntityPropertiesBoolean *checkMaterialProperties = dynamic_cast<EntityPropertiesBoolean*>(mesh->getProperties().getProperty("Check for missing materials"));

	EntityPropertiesBoolean *useMeshPriorities = dynamic_cast<EntityPropertiesBoolean*>(mesh->getProperties().getProperty("Use mesh priorities"));
	assert(useMeshPriorities != nullptr);

	EntityPropertiesBoolean *mergeMeshShapes = dynamic_cast<EntityPropertiesBoolean*>(mesh->getProperties().getProperty("Merge shapes"));
	assert(mergeMeshShapes != nullptr);

	EntityPropertiesDouble *geometryTolerance = dynamic_cast<EntityPropertiesDouble*>(mesh->getProperties().getProperty("Geometric tolerance"));

	EntityPropertiesBoolean *curvature = dynamic_cast<EntityPropertiesBoolean*>(mesh->getProperties().getProperty("Curvature refinement"));
	assert(curvature != nullptr);

	EntityPropertiesInteger *stepsPerCircle = dynamic_cast<EntityPropertiesInteger*>(mesh->getProperties().getProperty("Number of steps per circle"));
	assert(stepsPerCircle != nullptr);

	EntityPropertiesDouble *minCurvatureRefRadiusProperty = dynamic_cast<EntityPropertiesDouble*>(mesh->getProperties().getProperty("Minimum curvature refinement radius"));
	assert(minCurvatureRefRadiusProperty != nullptr);

	EntityPropertiesSelection *meshAlgorithm2D = dynamic_cast<EntityPropertiesSelection*>(mesh->getProperties().getProperty("2D mesh algorithm"));
	assert(meshAlgorithm2D != nullptr);

	EntityPropertiesSelection *meshAlgorithm3D = dynamic_cast<EntityPropertiesSelection*>(mesh->getProperties().getProperty("3D mesh algorithm"));

	EntityPropertiesBoolean *optimizeMesh = dynamic_cast<EntityPropertiesBoolean*>(mesh->getProperties().getProperty("Optimization"));

	EntityPropertiesSelection *proximityMeshingProperty = dynamic_cast<EntityPropertiesSelection*>(mesh->getProperties().getProperty("Proximity meshing"));

	EntityPropertiesDouble *refinementStepProperty = dynamic_cast<EntityPropertiesDouble*>(mesh->getProperties().getProperty("Maximum edge length for refinements"));
	assert(refinementStepProperty != nullptr);

	EntityPropertiesDouble *refinementRadiusProperty = dynamic_cast<EntityPropertiesDouble*>(mesh->getProperties().getProperty("Radius for point refinements"));
	assert(refinementRadiusProperty != nullptr);

	EntityPropertiesString *refinementListProperty = dynamic_cast<EntityPropertiesString*>(mesh->getProperties().getProperty("List of refinements"));
	assert(refinementListProperty != nullptr);

	EntityPropertiesDouble *stepsAlongDiagonalProperty = dynamic_cast<EntityPropertiesDouble*>(mesh->getProperties().getProperty("Number of steps along diagonal"));
	assert(stepsAlongDiagonalProperty != nullptr);

	EntityPropertiesDouble *maximumDeviationProperty = dynamic_cast<EntityPropertiesDouble*>(mesh->getProperties().getProperty("Maximum deviation of mesh from shape"));
	assert(maximumDeviationProperty != nullptr);

	EntityPropertiesBoolean *verboseProperty = dynamic_cast<EntityPropertiesBoolean*>(mesh->getProperties().getProperty("Verbose"));
	assert(verboseProperty != nullptr);

	EntityPropertiesBoolean *distanceBasedRefinementProperty = dynamic_cast<EntityPropertiesBoolean*>(mesh->getProperties().getProperty("Distance based refinement"));

	EntityPropertiesSelection *boundingSphereMode = dynamic_cast<EntityPropertiesSelection*>(mesh->getProperties().getProperty("Bounding sphere mode"));

	EntityPropertiesDouble *boundingSphereRadiusProperty = dynamic_cast<EntityPropertiesDouble*>(mesh->getProperties().getProperty("Bounding sphere radius"));
	assert(boundingSphereRadiusProperty != nullptr);

	EntityPropertiesDouble *boundingSphereNumberSteps = dynamic_cast<EntityPropertiesDouble*>(mesh->getProperties().getProperty("Steps on bounding sphere circumference"));

	EntityPropertiesEntityList *boundingSphereMaterialProperty = dynamic_cast<EntityPropertiesEntityList*>(mesh->getProperties().getProperty("Bounding sphere material"));
	assert(boundingSphereMaterialProperty != nullptr);

	EntityPropertiesSelection *elementOrderProperty = dynamic_cast<EntityPropertiesSelection*>(mesh->getProperties().getProperty("Element order"));

	maximumEdgeLengthValue	= maximumEdgeLengthProperty->getValue();
	meshOptimization		= (optimizeMesh == nullptr) ? true : optimizeMesh->getValue();
	curvatureRefinement		= curvature->getValue();
	numberOfStepsPerCircle	= stepsPerCircle->getValue();
	minCurvatureRefRadius	= minCurvatureRefRadiusProperty->getValue();
	usePriorities			= useMeshPriorities->getValue();
	mergeShapes				= mergeMeshShapes->getValue();
	refinementStep			= refinementStepProperty->getValue();
	refinementRadius		= refinementRadiusProperty->getValue();
	refinementList			= refinementListProperty->getValue();
	boundingSphereRadius	= boundingSphereRadiusProperty->getValue();
	stepsAlongDiagonal		= stepsAlongDiagonalProperty->getValue();
	maximumDeviation		= maximumDeviationProperty->getValue();
	verbose					= verboseProperty->getValue();

	checkMaterial = false;
	if (checkMaterialProperties != nullptr) checkMaterial = checkMaterialProperties->getValue();

	boundingSphereAbsolute = true;
	if (boundingSphereMode != nullptr)
	{
		if (boundingSphereMode->getValue() == "Relative") boundingSphereAbsolute = false;
	}

	if      (meshAlgorithm2D->getValue() == "MeshAdapt (most robust)")			meshAlg2D = 1;
	else if (meshAlgorithm2D->getValue() == "Automatic")						meshAlg2D = 2;
	else if (meshAlgorithm2D->getValue() == "Delauney (fastest)")				meshAlg2D = 5;
	else if (meshAlgorithm2D->getValue() == "Frontal-Delauney (high quality)")	meshAlg2D = 6;
	else if (meshAlgorithm2D->getValue() == "BAMG (anisotropic)")				meshAlg2D = 7;
	else assert(0);

	meshAlg3D = 1;
	if (meshAlgorithm3D != nullptr)
	{
		if      (meshAlgorithm3D->getValue() == "Delauney") meshAlg3D = 1;
		else if (meshAlgorithm3D->getValue() == "HXT"     ) meshAlg3D = 10;
		else assert(0);
	}

	proximityMeshing = 0;
	if (proximityMeshingProperty != nullptr)
	{
		if      (proximityMeshingProperty->getValue() == "None"                    ) proximityMeshing = 0;
		else if (proximityMeshingProperty->getValue() == "Check Self-intersections") proximityMeshing = 1;
		else if (proximityMeshingProperty->getValue() == "Fix Self-intersections"  ) proximityMeshing = 2;
	}

	if (elementOrderProperty != nullptr)
	{
		if      (elementOrderProperty->getValue() == "Low (1)"	 ) elementOrder = 1;
		else if (elementOrderProperty->getValue() == "Medium (2)") elementOrder = 2;
		else if (elementOrderProperty->getValue() == "High (3)"  ) elementOrder = 3;
	}

	meshStepsOnBoundingSphere = 12.0;
	if (boundingSphereNumberSteps != nullptr) meshStepsOnBoundingSphere = boundingSphereNumberSteps->getValue();

	tolerance = 0.0;
	if (geometryTolerance != nullptr) tolerance = geometryTolerance->getValue();

	useDistanceForVolumeMeshRefinement = false;
	if (distanceBasedRefinementProperty != nullptr) useDistanceForVolumeMeshRefinement = distanceBasedRefinementProperty->getValue();

	boundingSphereMaterial = boundingSphereMaterialProperty;
}




