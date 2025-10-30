// @otlicense
// File: ProximityMeshing.cpp
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


//////////////////////////////////////////////////////////////////////////////////////////////
// Please note: This code needs to be refactored
//////////////////////////////////////////////////////////////////////////////////////////////

#include "ProximityMeshing.h"

#include "Application.h"
#include "SelfIntersectionCheck.h"
#include "ProgressLogger.h"
#include "StepWidthManager.h"

#include "EntityAnnotation.h"
#include "EntityAnnotationData.h"

#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

#include <gmsh.h_cwrap>

#include <iostream>
#include <algorithm>

ProximityMeshing::ProximityMeshing(Application *app) : 
	application(app)
{
}

bool ProximityMeshing::fixMeshIntersections(const std::string &meshName, int proximityMeshing, ProgressLogger *logger, StepWidthManager *stepWidthManager, bool verbose)
{
	SelfIntersectionCheck intersectionChecker;
	bool hasSelfIntersections = false;

	std::vector < std::tuple<double, double, double, uint32_t, uint32_t> > IntersectingInfo;
	loadSelfIntersectionObject(intersectionChecker);

	application->getUiComponent()->setProgressInformation("Step 2/3: Check for self-intersections", true);

	hasSelfIntersections = checkSelfIntersection(intersectionChecker);

	if (!hasSelfIntersections) 
	{
		application->getUiComponent()->displayMessage(" No self intersections found\n");
	}
	else if (proximityMeshing == 1 && hasSelfIntersections) 
	{
		IntersectingInfo = intersectionChecker.TrisSelfIntersecting();
		visualiseSelfIntersections(meshName, IntersectingInfo, errorAnnotations);

		application->getUiComponent()->displayMessage("\nERROR: Self-intersections are detected in the surface mesh, so no volume mesh can be created. Please consider refining the mesh.\n");

		return false;
	}

	if (proximityMeshing == 2 && hasSelfIntersections)
	{
		IntersectingInfo = intersectionChecker.TrisSelfIntersecting();

		for (int ipass = 1; ipass <= 5; ipass++)
		{
			std::map<int, std::map<int, double>> faceToNeighborDistanceMap;
			std::map<int, std::map<int, double>> faceToNeighborCurvatureMap;
			std::map<int, double> faceCurvature;

			for (auto point : IntersectingInfo)
			{
				int firstFace = std::get<3>(point);
				int secondFace = std::get<4>(point);

				// Project the point on the first and second face and determine the distance of the projected points -> distance of the faces

				std::vector<double> IntersectingPoint(3), firstFaceProjectedPoint, firstFaceProjectedParamPoint, secondFaceProjectedPoint, secondFaceProjectedParamPoint;
				std::vector<double> firstFaceParamCoords, firstFaceCurvature, secondFaceParamCoords, secondFaceCurvature;

				// Loading the point to a new variable 
				IntersectingPoint[0] = std::get<0>(point);
				IntersectingPoint[1] = std::get<1>(point);
				IntersectingPoint[2] = std::get<2>(point);

				//Projecting the point on faces
				gmsh::model::getClosestPoint(2, firstFace, IntersectingPoint, firstFaceProjectedPoint, firstFaceProjectedParamPoint);
				gmsh::model::getClosestPoint(2, secondFace, IntersectingPoint, secondFaceProjectedPoint, secondFaceProjectedParamPoint);

				//Calculating Distance between Projected Points
				double Delta = sqrt(pow((firstFaceProjectedPoint[0] - secondFaceProjectedPoint[0]), 2) + pow((firstFaceProjectedPoint[1] - secondFaceProjectedPoint[1]), 2) + pow((firstFaceProjectedPoint[2] - secondFaceProjectedPoint[2]), 2));

				faceToNeighborDistanceMap[firstFace][secondFace] = Delta;
				faceToNeighborDistanceMap[secondFace][firstFace] = Delta;

				// Calculating the curvature on first Face
				gmsh::model::getParametrization(2, firstFace, IntersectingPoint, firstFaceParamCoords);
				gmsh::model::getCurvature(2, firstFace, firstFaceParamCoords, firstFaceCurvature);
				faceToNeighborCurvatureMap[firstFace][secondFace] = firstFaceCurvature[0];

				//Calculating the curvature on Second Face
				gmsh::model::getParametrization(2, secondFace, IntersectingPoint, secondFaceParamCoords);
				gmsh::model::getCurvature(2, secondFace, secondFaceParamCoords, secondFaceCurvature);
				faceToNeighborCurvatureMap[secondFace][firstFace] = secondFaceCurvature[0];

				if (faceCurvature.count(firstFace) > 0)
				{
					faceCurvature[firstFace] = std::max(faceCurvature[firstFace], firstFaceCurvature[0]);
				}
				else
				{
					faceCurvature[firstFace] = firstFaceCurvature[0];
				}

				if (faceCurvature.count(secondFace) > 0)
				{
					faceCurvature[secondFace] = std::max(faceCurvature[firstFace], secondFaceCurvature[0]);
				}
				else
				{
					faceCurvature[secondFace] = secondFaceCurvature[0];
				}
			}

			// Add the refinement fields

			for (auto face : faceToNeighborDistanceMap)
			{
				if (faceCurvature[face.first] != 0) {


					for (auto partner : face.second)
					{
						std::vector<double> partnerFaces;
						partnerFaces.push_back(partner.first);

						//std::cout << "Patner faces distance is" << faceToNeighborDistanceMap[face.first][partner.first];
						double Sdelta = faceToNeighborDistanceMap[face.first][partner.first];
						if (Sdelta == 0) {
							continue;
						}
						double radius = 1 / faceCurvature[face.first];
						if ((2 * radius) < (Sdelta / 3)) {
							continue;
						}
						double localSize = 2 * sqrt(2.0 *(Sdelta / 3)*radius - (Sdelta / 3)*(Sdelta / 3));

						std::string expression = quadraticExpression(localSize, Sdelta, partnerFaces);

						//std::string expression = linearExpression(localSize, Sdelta, partnerFaces);

						std::vector<double> faceList;
						faceList.push_back(face.first);

						gmsh::vectorpair dimTags;
						dimTags.push_back(std::pair<int, int>(2, face.first));

						gmsh::vectorpair edgeTags;
						gmsh::model::getBoundary(dimTags, edgeTags, false, false);

						std::vector<double> edgeList;
						edgeList.reserve(edgeTags.size());

						for (auto tag : edgeTags)
						{
							assert(tag.first == 1);
							edgeList.push_back(tag.second);
						}

						stepWidthManager->addProximityRefinementField(expression, edgeList, faceList);
					}
				}

			}

			stepWidthManager->setProximityRefinementFields();

			gmsh::model::mesh::clear();  // We do not need to clear the entire mesh, removing the affected faces and edges would be sufficient

			application->getUiComponent()->setProgressInformation("Step 2/3: Regenerating surface mesh", false);
			application->getUiComponent()->setProgress(0);

			logger->startLogger2D(verbose);

			try
			{
				gmsh::model::mesh::generate(2);
			}
			catch (int ierr)
			{
				throw std::string("ERROR in surface mesh generation. Error code: " + std::to_string(ierr) + "\n");
			}
			catch (std::string error)
			{
				throw std::string("ERROR in surface mesh generation. Error code: " + error + "\n");
			}
			catch (...)
			{
				throw std::string("ERROR in surface mesh generation. Error code: unknown\n");
			}

			logger->stopLogger2D();

			intersectionChecker.clear();
			loadSelfIntersectionObject(intersectionChecker);
			hasSelfIntersections = checkSelfIntersection(intersectionChecker);

			if (!hasSelfIntersections)
			{
				IntersectingInfo = intersectionChecker.TrisSelfIntersecting();
				visualiseSelfIntersections(meshName, IntersectingInfo, errorAnnotations);
				break;
			}
			else 
			{
				gmsh::option::setNumber("Mesh.AngleToleranceFacetOverlap", 0.01);
				IntersectingInfo = intersectionChecker.TrisSelfIntersecting();
				visualiseSelfIntersections(meshName, IntersectingInfo, errorAnnotations);
			}
		}
	}

	return (!hasSelfIntersections); // return whether the mesh is valid
}

void ProximityMeshing::loadSelfIntersectionObject(SelfIntersectionCheck &intersectionChecker)
{
	// Read all nodes from the mesh i.e., dim=-1 and tag=-1 (default)
	std::vector<size_t> nTags;
	std::vector<double> nCoords;
	std::vector<double> parametricCoord;
	gmsh::model::mesh::getNodes(nTags, nCoords, parametricCoord);

	// TODO: store the nodes in the self intersection checker

	for (uint32_t i = 0; i < nTags.size(); i++)
	{
		intersectionChecker.setNodes((uint32_t) nTags[i], nCoords[3 * i], nCoords[3 * i + 1], nCoords[3 * i + 2]);
	}

	// Get all faces from the model
	gmsh::vectorpair faceTags;
	gmsh::model::getEntities(faceTags, 2);

	// Now we loop over all faces in the model and get their triangles
	std::vector<int> elementTypes2;
	std::vector<std::vector<size_t>> elementTags2;
	std::vector<std::vector<size_t>> nodeTags2;
	for (auto faceTag : faceTags)
	{
		elementTypes2.clear();
		elementTags2.clear();
		nodeTags2.clear();

		gmsh::model::mesh::getElements(elementTypes2, elementTags2, nodeTags2, faceTag.first, faceTag.second);

		assert(faceTag.first >= 0);
		assert(faceTag.second >= 0);
		bool faceInverted = (faceTag.second < 0);
		assert(!faceInverted);

		if (elementTypes2.size() > 0)
		{
			// Check and store the surface mesh 
			assert(elementTypes2.size() == 1);
			assert(elementTags2.size() == 1);
			assert(nodeTags2.size() == 1);

			size_t numberTriangles = elementTags2[0].size();
			assert(nodeTags2[0].size() == 3 * numberTriangles);

			if (nodeTags2[0].size() == 3 * numberTriangles)
			{
				for (size_t i = 0; i < numberTriangles; i++)
				{
					uint32_t node1 = (uint32_t) nodeTags2[0][3 * i];
					uint32_t node2 = (uint32_t) nodeTags2[0][3 * i + 1];
					uint32_t node3 = (uint32_t) nodeTags2[0][3 * i + 2];

					// Add the triangle (together with its tag) to the self intersection checker
					intersectionChecker.setTriangles((uint32_t) faceTag.second, (uint32_t) elementTags2[0][i], node1, node2, node3);
				}
			}
		}
	}
}

bool ProximityMeshing::checkSelfIntersection(SelfIntersectionCheck &checker)
{
	// Perform the self intersection check and determine the locations of the self intersections and the 
	// affected surface tags

	bool checkSelfIntersect = checker.IsSelfIntersecting();

	return checkSelfIntersect;
}

void ProximityMeshing::visualiseSelfIntersections(const std::string &meshName, std::vector < std::tuple<double, double, double, uint32_t, uint32_t> > IntersectingInfo, std::list<EntityAnnotation *> &errorAnnotation)
{
	EntityAnnotation *annotation = new EntityAnnotation(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, application->getServiceName());
	
	if (IntersectingInfo.size() != 0) 
	{
		for (int i = 0; i < IntersectingInfo.size(); i++)
		{
			annotation->addPoint(std::get<0>(IntersectingInfo[i]), std::get<1>(IntersectingInfo[i]), std::get<2>(IntersectingInfo[i]), 1.0, 0.0, 0.0);
		}

		annotation->setName(meshName + "/Mesh Errors/Self-intersections");
		annotation->setInitiallyHidden(true);

		errorAnnotation.push_back(annotation);
	}
	else 
	{
		for (auto item : errorAnnotation)
		{
			delete item;
		}
		errorAnnotation.clear();
	}
}

std::string ProximityMeshing::quadraticExpression(double localSize, double Sdelta, std::vector<double> partnerFaces)
{
	//Quadratic Expression 
	double y2 = 1.0042 * localSize;
	std::string f1 = std::to_string((y2 - localSize) / ((Sdelta)*(Sdelta)));
	std::string f2 = std::to_string((localSize));
	std::string delta = std::to_string((Sdelta));
	int distanceField = gmsh::model::mesh::field::add("Distance");
	gmsh::model::mesh::field::setNumbers(distanceField, "FacesList", partnerFaces);
	std::string expression = f1 + " * (F" + std::to_string(distanceField) + "-" + delta + ")^2 + " + f2;
	//std::cout << "\n distance field is " << distanceField << std::endl;
	//std::cout << "expression is " << expression << std::endl;

	return expression;
}

std::string ProximityMeshing::linearExpression(double localSize, double Sdelta, std::vector<double> partnerFaces)
{
	//Linear Expression 
	double y2 = 1.06* localSize;
	std::string f1 = std::to_string((y2 - localSize) / (Sdelta));
	std::string f2 = std::to_string((y2 - 2 * localSize));
	int distanceField = gmsh::model::mesh::field::add("Distance");
	//gmsh::model::mesh::field::setNumbers(distanceField, "EdgesList", it->second.edgeList);
	gmsh::model::mesh::field::setNumbers(distanceField, "FacesList", partnerFaces);
	std::string expression = "F" + std::to_string(distanceField) + "* (" + f1 + ") - (" + f2 + ")";
	//std::cout << "\n distance field is " << distanceField << std::endl;
	//std::cout << "expression is " << expression << std::endl;

	return expression;
}

void ProximityMeshing::storeErrorAnnotations(void)
{
	for (auto annotation : errorAnnotations)
	{
		annotation->storeToDataBase();
		application->getModelComponent()->addNewTopologyEntity(annotation->getEntityID(), annotation->getEntityStorageVersion(), true);
		application->getModelComponent()->addNewDataEntity(annotation->getAnnotationData()->getEntityID(), 
														annotation->getAnnotationData()->getEntityStorageVersion(), 
														annotation->getEntityID());		
	}
}

