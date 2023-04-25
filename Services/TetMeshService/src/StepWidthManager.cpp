#include "StepWidthManager.h"

#include "Application.h"
#include "Properties.h"
#include "ObjectManager.h"

#include "BoundingBox.h"
#include "EntityBase.h"
#include "GeometryOperations.h"

#include "OpenTwinFoundation/UiComponent.h"

#define _USE_MATH_DEFINES

#include <math.h>

#include "TopExp_Explorer.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS.hxx"

#include <gmsh_cwrap.h>

StepWidthManager::StepWidthManager(Application *app) :
	application(app),
	maximumEdgeLength(0.0),
	boundingSphereMeshStep(0.0),
	backgroundBaseStep(0.0),
	backgroundDistance(0.0),
	backgroundMeshRefinementField(-1)
{
}

void StepWidthManager::determineBaseStepWidths(BoundingBox &boundingBox, Properties &properties)
{
	maximumEdgeLength = 0.0;

	if (properties.getStepsAlongDiagonal() > 0)
	{
		double maximumEdgeLengthFromDiagonal = boundingBox.getDiagonal() / properties.getStepsAlongDiagonal();
		maximumEdgeLength = std::min(properties.getMaximumEdgeLengthValue(), maximumEdgeLengthFromDiagonal);
	}
	else
	{
		maximumEdgeLength = properties.getMaximumEdgeLengthValue();
	}

	double sphereRadius = properties.getBoundingSphereRadius();

	if (!properties.getBoundingSphereAbsolute())
	{
		sphereRadius *= 0.5 * boundingBox.getDiagonal();
	}	
	
	boundingSphereMeshStep = 2 * M_PI * sphereRadius / properties.getMeshStepsOnBoundingSphere();

	if (properties.getBoundingSphereRadius() > 0.5 * boundingBox.getDiagonal())
	{
		// A bounding sphere will be added (ensure that the base mesh step is small enough)
		maximumEdgeLength = std::min(maximumEdgeLength, boundingSphereMeshStep);
	}

	backgroundBaseStep = boundingSphereMeshStep;
	backgroundDistance = sphereRadius;

	if (sphereRadius == 0.0)
	{
		// We need to derive the background parameters from the default settings
		backgroundDistance = boundingBox.getDiagonal();
		backgroundBaseStep = M_PI * boundingBox.getDiagonal() / 10;
	}
}

void StepWidthManager::buildIndexedFaceToStepSizeVector(EntityBase *entity, const TopoDS_Shape *shape, Properties &properties)
{
	faceStepWidthVector.clear();

	// Determine the curvature refinement settings for this shape

	bool   localCurvatureRefinement    = properties.getCurvatureRefinement();
	int    localNumberOfStepsPerCircle = properties.getNumberOfStepsPerCircle();
	double localMinCurvatureRefRadius  = properties.getMinCurvatureRefRadius();
	double localMaximumDeviation       = properties.getMaximumDeviation();

	getLocalMeshPropertiesForEntity(entity, localCurvatureRefinement, localNumberOfStepsPerCircle, localMinCurvatureRefRadius, localMaximumDeviation);

	TopExp_Explorer exp;

	// Count the number of faces
	size_t count = 0;
	for (exp.Init(*shape, TopAbs_FACE); exp.More(); exp.Next()) count++;

	// Reserve the size of the vector
	faceStepWidthVector.reserve(count);

	// Loop through all faces of the entity and store the maximum stepwidth for each face in the vector
	for (exp.Init(*shape, TopAbs_FACE); exp.More(); exp.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(exp.Current());

		double stepWidth = getMaxStepWidthForFace(aFace, localCurvatureRefinement, localNumberOfStepsPerCircle, localMinCurvatureRefRadius, localMaximumDeviation);

		faceStepWidthVector.push_back(stepWidth);
	}
}

void StepWidthManager::getLocalMeshPropertiesForEntity(EntityBase *entity, 
													   bool &localCurvatureRefinement, int &localNumberOfStepsPerCircle, 
													   double &localMinCurvatureRefRadius, double &localMaximumDeviation)
{
	EntityPropertiesSelection *meshRefinmentProp = dynamic_cast<EntityPropertiesSelection*>(entity->getProperties().getProperty("Mesh refinement"));
	if (meshRefinmentProp != nullptr)
	{
		std::string refinementSetting = meshRefinmentProp->getValue();

		if (refinementSetting == "Local setting")
		{
			EntityPropertiesBoolean *curvatureRefinment = dynamic_cast<EntityPropertiesBoolean*>(entity->getProperties().getProperty("Curvature refinement"));
			if (curvatureRefinment != nullptr)
			{
				if (!curvatureRefinment->getValue())
				{
					localCurvatureRefinement = false;
				}
				else
				{
					localCurvatureRefinement = true;

					EntityPropertiesInteger *numStepsPerCircle = dynamic_cast<EntityPropertiesInteger*>(entity->getProperties().getProperty("Number of steps per circle"));
					if (numStepsPerCircle != nullptr) localNumberOfStepsPerCircle = numStepsPerCircle->getValue();

					EntityPropertiesDouble *minCurvRefinementRadius = dynamic_cast<EntityPropertiesDouble*>(entity->getProperties().getProperty("Minimum curvature refinement radius"));
					if (minCurvRefinementRadius != nullptr) localMinCurvatureRefRadius = minCurvRefinementRadius->getValue();

					EntityPropertiesDouble *maximumDeviation = dynamic_cast<EntityPropertiesDouble*>(entity->getProperties().getProperty("Maximum deviation of mesh from shape"));
					if (maximumDeviation != nullptr) localMaximumDeviation = maximumDeviation->getValue();
				}
			}
		}
	}
}

double StepWidthManager::getMaxStepWidthForFace(TopoDS_Face &face, 
												bool localCurvatureRefinement, int localNumberOfStepsPerCircle, 
												double localMinCurvatureRefRadius, double localMaximumDeviation)
{
	double stepWidth = 0.0;

	if (localCurvatureRefinement)
	{
		double maxCurvature = GeometryOperations::getMaximumFaceCurvature(face);

		if (maxCurvature > 0.0)
		{
			double radius = 1.0 / maxCurvature;

			if (radius >= localMinCurvatureRefRadius)
			{
				stepWidth = 2 * M_PI * radius / localNumberOfStepsPerCircle;
			}
		}

		if (localMaximumDeviation > 0.0 && maxCurvature > 0.0)
		{
			// Determine the required step width based on the maximum deviation constraint
			// for the face curvature

			double radius = 1.0 / maxCurvature;

			if (radius > localMaximumDeviation)
			{
				double stepWidthFromMaxDeviation = 2.0 * sqrt(2.0 * localMaximumDeviation * radius - localMaximumDeviation * localMaximumDeviation);

				if (stepWidth == 0.0)
				{
					stepWidth = stepWidthFromMaxDeviation;
				}
				else
				{
					stepWidth = std::min(stepWidth, stepWidthFromMaxDeviation);
				}
			}
		}
	}

	return stepWidth;
}

void StepWidthManager::buildTagToAnnotationMap(gmsh::vectorpair &output)
{
	// Loop through all faces in the output and set the step size for the face in the map
	int faceIndex = 0;
	for (auto tag : output)
	{
		if (tag.first == 2)
		{
			if (faceStepWidthVector[faceIndex] > 0)
			{
				if (faceTagToStepSizeMap.count(tag.second) > 0)
				{
					// We have multiple definitions for one face -> choose the smallest setting
					faceTagToStepSizeMap[tag.second] = std::min(faceStepWidthVector[faceIndex], faceTagToStepSizeMap[tag.second]);
				}
				else
				{
					// We do not have a definition for this face yet -> use the setting
					faceTagToStepSizeMap[tag.second] = faceStepWidthVector[faceIndex];
				}
			}

			faceIndex++;
		}
	}
}

void StepWidthManager::applyVolumeMeshStepWidthToEntity(EntityBase *entity, ObjectManager &objectManager, Properties &properties)
{
	// Determine the base mesh step width for this entity (if specified)
	EntityPropertiesDouble *meshStepWidth = dynamic_cast<EntityPropertiesDouble*>(entity->getProperties().getProperty("Maximum edge length"));
	if (meshStepWidth == nullptr)
	{
		// This is for backward compatibility with oder models only.
		meshStepWidth = dynamic_cast<EntityPropertiesDouble*>(entity->getProperties().getProperty("Mesh step width"));
	}

	double shapeMeshStepWidth = getMaximumEdgeLength();

	if (meshStepWidth != nullptr)
	{
		if (meshStepWidth->getValue() > 0.0)
		{
			shapeMeshStepWidth = meshStepWidth->getValue();
		}
	}

	applyVolumeMeshStepWidthToObjects(objectManager.getMeshElementsForEntity(entity), shapeMeshStepWidth);
}

void StepWidthManager::applyVolumeMeshStepWidthToObjects(gmsh::vectorpair &shapeTags, double stepWidth)
{
	// Determine the list of faces and edges of the entity
	std::set<int> faceSet;
	std::set<int> edgeSet;
	std::set<int> volumeSet;

	for (auto tag : shapeTags)
	{
		switch (tag.first)
		{
		case 1:
			edgeSet.insert(tag.second);
			break;
		case 2:
			faceSet.insert(tag.second);
			break;
		case 3:
			volumeSet.insert(tag.second);
			break;
		}
	}

	// Create a new refinement information element for this set of edges, faces, volumes
	createRefinement(stepWidth, faceSet, edgeSet, volumeSet);
}

void StepWidthManager::applyCurvatureMeshStepWidthToEntities(void)
{
	for (std::map<int, double>::iterator it = faceTagToStepSizeMap.begin(); it != faceTagToStepSizeMap.end(); it++)
	{
		std::list<int> faceList;
		faceList.push_back(it->first);

		double faceStep = it->second;

		if (faceStep > 0.0 && faceStep < maximumEdgeLength)
		{
			applyMeshStepWidthToFaces(faceList, faceStep);
		}
	}
}

void StepWidthManager::applyMeshStepWidthToFaces(std::list<int> &refineFaceTagList, double stepWidth)
{
	if (refineFaceTagList.empty()) return;

	// Determine the list of faces and edges of the entity
	std::set<int> faceSet;
	std::set<int> edgeSet;
	std::set<int> volumeSet;

	gmsh::vectorpair faceTags;
	for (auto face : refineFaceTagList)
	{
		faceTags.push_back(std::pair<int, int>(2, face));
		faceSet.insert(face);
	}

	gmsh::vectorpair edgeTags;
	gmsh::model::getBoundary(faceTags, edgeTags, false, false);

	for (auto tag : edgeTags)
	{
		assert(tag.first == 1);
		edgeSet.insert(tag.second);
	}

	// Create a new refinement information element for this set of edges, faces, volumes
	createRefinement(stepWidth, faceSet, edgeSet, volumeSet);
}

void StepWidthManager::createRefinement(double stepWidth, std::set<int> &faceSet, std::set<int> &edgeSet, std::set<int> &volumeSet)
{
	// Now create a new refinement and add the entity information as vectors
	std::string stepWidthString = std::to_string(stepWidth);

	meshRefinement refinement = meshFields[stepWidthString];

	refinement.stepWidth = stepWidth;
	refinement.edgeSet.insert(edgeSet.begin(), edgeSet.end());
	refinement.faceSet.insert(faceSet.begin(), faceSet.end());
	refinement.volumeSet.insert(volumeSet.begin(), volumeSet.end());

	meshFields[stepWidthString] = refinement;
}

void StepWidthManager::convertSet(const std::set<int> &setData, std::vector<double> &listData)
{
	if (!setData.empty())
	{
		listData.reserve(setData.size());
		for (auto item : setData)
		{
			listData.push_back(1.0 * item);
		}
	}
}

void StepWidthManager::applyUserDefinedRefinmentsToFacesAndPoints(double refinementStep, std::string refinementList)
{
	if (refinementStep == 0.0 || refinementList.empty()) return; // Nothing to do here

	std::string refinement = getRefinementFromString(refinementList);
	std::list<int> faceList;

	while (!refinement.empty())
	{
		double x = 0.0, y = 0.0, z = 0.0;
		int faceId = -1;

		if (getPointFromRefinement(refinement, x, y, z))
		{
			// We have a point refinement specified
			std::array<double, 3> point = { x, y, z };
			refinementPoints.push_back(point);
		}
		else if (getFaceIdFromRefinement(refinement, faceId))
		{
			// We have a face id refinement specified
			faceList.push_back(faceId);
		}
		else
		{
			application->uiComponent()->displayMessage("WARNING: Invalid mesh refinement specification: " + refinement + "\n");
		}

		refinement = getRefinementFromString(refinementList);
	}

	applyMeshStepWidthToFaces(faceList, refinementStep);
}

std::string StepWidthManager::getRefinementFromString(std::string &refinementString)
{
	// Now read the characters from the string until either the end of the string is reached or a ";" is encountered
	std::string refinement;

	size_t endPos = refinementString.find(';');

	if (endPos == std::string::npos)
	{
		// No ; found -> return the enture string
		refinement = refinementString;
		refinementString = "";
	}
	else
	{
		// A ; was found. Extract the first part of the string until the ;
		refinement = refinementString.substr(0, endPos);
		refinementString = refinementString.substr(endPos + 1);
	}

	return refinement;
}

bool StepWidthManager::getPointFromRefinement(const std::string &refinement, double &x, double &y, double &z)
{
	std::string refString = refinement;
	for (size_t i = 0; i < refString.size(); i++) if (refString[i] == '(' || refString[i] == ')' || refString[i] == ',') refString[i] = ' ';

	std::stringstream ss(refString);

	ss >> x;
	if (ss.fail()) return false;

	ss >> y;
	if (ss.fail()) return false;

	ss >> z;
	if (ss.fail()) return false;

	return true;
}

bool StepWidthManager::getFaceIdFromRefinement(const std::string &refinement, int &faceId)
{
	std::stringstream ss(refinement);

	ss >> faceId;
	if (ss.fail()) return false;

	return true;
}

void StepWidthManager::apply2DRefinementsToMesher(double refinementStep, double refinementRadius)
{
	refinementFields2D.clear();

	// Create a new field for each refinement entry
	for (std::map<std::string, meshRefinement>::iterator it = meshFields.begin(); it != meshFields.end(); it++)
	{
		// Here we do not used the distance based meshing, even if it is turned on. We use the density from edges
		// propagation anyway and the distance based refinement would affect the performance significantly.

		std::vector<double> edgeList;
		std::vector<double> faceList;

		convertSet(it->second.edgeSet, edgeList);
		convertSet(it->second.faceSet, faceList);

		int evalField = gmsh::model::mesh::field::add("MathEval");
		gmsh::model::mesh::field::setString(evalField, "F", std::to_string(it->second.stepWidth));

		int restrictField = gmsh::model::mesh::field::add("Restrict");
		gmsh::model::mesh::field::setNumber(restrictField, "IField", evalField);
		gmsh::model::mesh::field::setNumbers(restrictField, "EdgesList", edgeList);
		gmsh::model::mesh::field::setNumbers(restrictField, "FacesList", faceList);

		refinementFields2D.push_back(restrictField);
	}

	// Create refinement fields (balls) for the points
	for (auto point : refinementPoints)
	{
		int restrictField = gmsh::model::mesh::field::add("Ball");

		gmsh::model::mesh::field::setNumber(restrictField, "Radius", refinementRadius);
		gmsh::model::mesh::field::setNumber(restrictField, "VIn", refinementStep);
		gmsh::model::mesh::field::setNumber(restrictField, "VOut", getMaximumEdgeLength());
		gmsh::model::mesh::field::setNumber(restrictField, "XCenter", point[0]);
		gmsh::model::mesh::field::setNumber(restrictField, "YCenter", point[1]);
		gmsh::model::mesh::field::setNumber(restrictField, "ZCenter", point[2]);

		refinementFields2D.push_back(restrictField);
	}

	// Add a background min mesh field which sets all refinements (if refinements are available)
	if (!refinementFields2D.empty())
	{
		backgroundMeshRefinementField = gmsh::model::mesh::field::add("Min");
		gmsh::model::mesh::field::setNumbers(backgroundMeshRefinementField, "FieldsList", refinementFields2D);

		gmsh::model::mesh::field::setAsBackgroundMesh(backgroundMeshRefinementField);
	}
}

void StepWidthManager::apply3DRefinementsToMesher(bool useDistanceForVolumeMeshRefinement, double refinementStep, double refinementRadius)
{
	if (useDistanceForVolumeMeshRefinement)
	{
		// If we use the distance based refinement, we need to re-apply the density field. This time, we also take into account the 
		// distance from the faces. This does not affect the performance of the volume mesh creation seriously. 

		std::vector<double> refinementFields;

		// Create a new field for each refinement entry
		for (std::map<std::string, meshRefinement>::iterator it = meshFields.begin(); it != meshFields.end(); it++)
		{
			std::vector<double> edgeList;
			std::vector<double> faceList;

			convertSet(it->second.edgeSet, edgeList);
			convertSet(it->second.faceSet, faceList);

			int distanceField = gmsh::model::mesh::field::add("Distance");
			gmsh::model::mesh::field::setNumbers(distanceField, "EdgesList", edgeList);
			gmsh::model::mesh::field::setNumbers(distanceField, "FacesList", faceList);

			if (it->second.stepWidth < backgroundBaseStep)
			{
				// First, we assign a refinement to the faces which gets coarser, the farther away we are from the faces
				std::string f1 = std::to_string((backgroundBaseStep - it->second.stepWidth) / backgroundDistance);
				std::string f2 = std::to_string(it->second.stepWidth);

				std::string expression = "F" + std::to_string(distanceField) + "*" + f1 + "+" + f2;

				int evalField = gmsh::model::mesh::field::add("MathEval");
				gmsh::model::mesh::field::setString(evalField, "F", expression);

				refinementFields.push_back(evalField);

				// Second, assign a volume refinement constraint

				std::vector<double> volumeList;
				convertSet(it->second.volumeSet, volumeList);

				evalField = gmsh::model::mesh::field::add("MathEval");
				gmsh::model::mesh::field::setString(evalField, "F", std::to_string(it->second.stepWidth));

				int restrictField = gmsh::model::mesh::field::add("Restrict");
				gmsh::model::mesh::field::setNumber(restrictField, "IField", evalField);
				gmsh::model::mesh::field::setNumbers(restrictField, "RegionsList", volumeList);

				refinementFields.push_back(restrictField);
			}
		}

		// Create refinement fields (balls) for the points

		for (auto point : refinementPoints)
		{
			int restrictField = gmsh::model::mesh::field::add("Ball");

			gmsh::model::mesh::field::setNumber(restrictField, "Radius", refinementRadius);
			gmsh::model::mesh::field::setNumber(restrictField, "VIn", refinementStep);
			gmsh::model::mesh::field::setNumber(restrictField, "VOut", getMaximumEdgeLength());
			gmsh::model::mesh::field::setNumber(restrictField, "XCenter", point[0]);
			gmsh::model::mesh::field::setNumber(restrictField, "YCenter", point[1]);
			gmsh::model::mesh::field::setNumber(restrictField, "ZCenter", point[2]);

			refinementFields.push_back(restrictField);
		}

		if (!refinementFields.empty())
		{
			// Add a new background min mesh field (the previous one will then be discarded)
			backgroundMeshRefinementField = gmsh::model::mesh::field::add("Min");
			gmsh::model::mesh::field::setNumbers(backgroundMeshRefinementField, "FieldsList", refinementFields);

			gmsh::model::mesh::field::setAsBackgroundMesh(backgroundMeshRefinementField);
		}
	}
	else
	{
		// We turn this option on for the 3D meshing to make sure that the volumes take the stepwidth from their bounding faces
		// (if we don't use the distance based refinement)

		gmsh::option::setNumber("Mesh.MeshSizeExtendFromBoundary", 1);
	}
}

void StepWidthManager::addProximityRefinementField(const std::string &expression, std::vector<double> &edgeList, std::vector<double> &faceList)
{
	int evalField = gmsh::model::mesh::field::add("MathEval");
	gmsh::model::mesh::field::setString(evalField, "F", expression);

	int restrictField = gmsh::model::mesh::field::add("Restrict");
	gmsh::model::mesh::field::setNumber(restrictField, "IField", evalField);
	gmsh::model::mesh::field::setNumbers(restrictField, "EdgesList", edgeList);
	gmsh::model::mesh::field::setNumbers(restrictField, "FacesList", faceList);

	refinementFields2D.push_back(restrictField);
}

void StepWidthManager::setProximityRefinementFields(void)
{
	if (backgroundMeshRefinementField == -1)
	{
		backgroundMeshRefinementField = gmsh::model::mesh::field::add("Min");
		gmsh::model::mesh::field::setAsBackgroundMesh(backgroundMeshRefinementField);
	}

	gmsh::model::mesh::field::setNumbers(backgroundMeshRefinementField, "FieldsList", refinementFields2D);
}
