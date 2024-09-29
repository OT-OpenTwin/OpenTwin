#include "FaceAnnotationsManager.h"

#include "Application.h"

#include "ClassFactoryCAD.h"
#include "ClassFactory.h"
#include "EntityFaceAnnotation.h"

#include "OTServiceFoundation/ModelComponent.h"

#include "TopExp_Explorer.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS.hxx"

#include <gmsh.h_cwrap>

void FaceAnnotationsManager::loadAllFaceAnnotations(void)
{
	std::list<ot::UID> annotationEntityIDs = application->modelComponent()->getIDsOfFolderItemsOfType("", "EntityFaceAnnotation", true);

	if (annotationEntityIDs.empty()) return;

	application->prefetchDocumentsFromStorage(annotationEntityIDs);



	for (auto annotation : annotationEntityIDs)
	{
		ot::UID entityID = annotation;
		ot::UID entityVersion = application->getPrefetchedEntityVersion(entityID);

		EntityFaceAnnotation *annotationEntity = dynamic_cast<EntityFaceAnnotation *>(application->modelComponent()->readEntityFromEntityIDandVersion(entityID, entityVersion, application->getClassFactory()));
		assert(annotationEntity != nullptr);

		if (annotationEntity != nullptr)
		{
			faceAnnotations.push_back(annotationEntity);
		}
	}
}

void FaceAnnotationsManager::buildEntityNameToAnnotationsMap(void)
{
	for (auto annotation : faceAnnotations)
	{
		std::list<std::string> geometryNames;
		annotation->getGeometryNames(geometryNames);

		for (auto name : geometryNames)
		{
			entityNametoAnnotationsMap[name].push_back(annotation);
		}
	}
}

void FaceAnnotationsManager::buildFaceToAnnotationMap(const std::string &entityName, EntityBrep *brep)
{
	faceToAnnotationMap.clear();

	// Loop through all face annotations for the given entity
	for (auto annotation : entityNametoAnnotationsMap[entityName])
	{
		// Get a list of faces for this annotation
		std::list<TopoDS_Shape> faceList = annotation->findFacesFromShape(brep);

		for (auto face : faceList)
		{
			// Now build a map of all faces pointing to all their attached annotations
			faceToAnnotationMap[face.TShape()].push_back(annotation);
		}
	}
}

void FaceAnnotationsManager::buildIndexedFaceToAnnotationVector(const TopoDS_Shape *shape)
{
	faceAnnotationVector.clear();

	// This function loops through all faces of the shape and builds a vector where each entry corresponds to a face index.
	// The vector maps to the list of annotations for this face

	TopExp_Explorer exp;

	// Count the number of faces
	size_t count = 0;
	for (exp.Init(*shape, TopAbs_FACE); exp.More(); exp.Next()) count++;

	// Now dimension the vector
	faceAnnotationVector.reserve(count);

	// Finally, assign the face annotations to the vector
	for (exp.Init(*shape, TopAbs_FACE); exp.More(); exp.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(exp.Current());

		faceAnnotationVector.push_back(faceToAnnotationMap[aFace.TShape()]);
	}
}

void FaceAnnotationsManager::buildTagToAnnotationMap(gmsh::vectorpair &output)
{
	// Loop through all faces in the output and set the annotations for the face in the map
	int faceIndex = 0;

	for (auto tag : output)
	{
		if (tag.first == 2)
		{
			faceTagToAnnotationMap[tag.second] = faceAnnotationVector[faceIndex];
			faceIndex++;
		}
	}
}

std::list<ot::UID> FaceAnnotationsManager::getAnnotationIDsforFace(int faceTag)
{
	std::list<ot::UID> annotationIDs;

	for (auto annotation : faceTagToAnnotationMap[faceTag])
	{
		annotationIDs.push_back(annotation->getEntityID());
	}

	return annotationIDs;
}

std::list<EntityFaceAnnotation *> FaceAnnotationsManager::getAnnotationsforFace(int faceTag)
{
	return faceTagToAnnotationMap[faceTag];
}

