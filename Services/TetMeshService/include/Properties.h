#pragma once

#include <string>

#include "BoundingBox.h"

class EntityMeshTet;
class EntityPropertiesEntityList;

class Properties
{
public:
	Properties();
	~Properties() {};

	void readSettings(EntityMeshTet *mesh);

	bool		getCheckMaterial(void) { return checkMaterial; }
	bool		getBoundingSphereAbsolute(void) { return boundingSphereAbsolute; }
	int			getMeshAlg2D(void) { return meshAlg2D; }
	int			getMeshAlg3D(void) { return meshAlg3D; }
	double		getMaximumEdgeLengthValue(void) { return maximumEdgeLengthValue; }
	bool		getMeshOptimization(void) { return meshOptimization; }
	int			getProximityMeshing(void) { return proximityMeshing; }
	bool		getCurvatureRefinement(void) { return curvatureRefinement; }
	int			getNumberOfStepsPerCircle(void) { return numberOfStepsPerCircle; }
	double		getMinCurvatureRefRadius(void) { return minCurvatureRefRadius; }
	bool		getUsePriorities(void) { return usePriorities; }
	bool		getMergeShapes(void) { return mergeShapes; }
	double		getRefinementStep(void) { return refinementStep; }
	double	    getRefinementRadius(void) { return refinementRadius; }
	std::string getRefinementList(void) { return refinementList; }
	double		getBoundingSphereRadius(void) { return boundingSphereRadius; }
	double		getStepsAlongDiagonal(void) { return stepsAlongDiagonal; }
	double		getMaximumDeviation(void) { return maximumDeviation; }
	bool		getVerbose(void) { return verbose; }
	double		getMeshStepsOnBoundingSphere(void) { return meshStepsOnBoundingSphere; }
	double		getTolerance(void) { return tolerance; }
	bool		getUseDistanceForVolumeMeshRefinement(void) { return useDistanceForVolumeMeshRefinement; }
	int			getElementOrder(void) { return elementOrder; }

	EntityPropertiesEntityList *getBoundingSphereMaterial(void) { return boundingSphereMaterial; }

private:
	bool		checkMaterial;
	bool		boundingSphereAbsolute;
	int			meshAlg2D;
	int			meshAlg3D;
	int			elementOrder;
	double		maximumEdgeLengthValue;
	bool		meshOptimization;
	int			proximityMeshing;
	bool		curvatureRefinement;
	int			numberOfStepsPerCircle;
	double		minCurvatureRefRadius;
	bool		usePriorities;
	bool		mergeShapes;
	double		refinementStep;
	double	    refinementRadius;
	std::string refinementList;
	double		boundingSphereRadius;
	double		stepsAlongDiagonal;
	double		maximumDeviation;
	bool		verbose;
	double		meshStepsOnBoundingSphere;
	double		tolerance;
	bool		useDistanceForVolumeMeshRefinement;

	EntityPropertiesEntityList *boundingSphereMaterial;
};




