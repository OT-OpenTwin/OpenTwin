#include "ShapeHealing.h"

#include "Application.h"
#include "EntityCache.h"
#include "ClassFactoryCAD.h"
#include "ClassFactory.h"

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include <map>


#include "TDataStd_Name.hxx"
#include "TopoDS_Iterator.hxx"
#include "BRepBuilderAPI_Transform.hxx"
#include "ShapeFix_Shape.hxx"
#include "ShapeFix_Wireframe.hxx"
#include "ShapeFix_FixSmallFace.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Solid.hxx"
#include "TopoDS.hxx"
#include "Quantity_Color.hxx"
#include "ShapeBuild_ReShape.hxx"
#include "BRep_Tool.hxx"
#include "BRepOffsetAPI_Sewing.hxx"
#include "BRepBuilderAPI_MakeSolid.hxx"
#include "BRepCheck_Analyzer.hxx"
#include "BRepLib.hxx"
#include "GProp_GProps.hxx"
#include "BRepGProp.hxx"
#include "BRepTools.hxx"
#include "Bnd_Box.hxx"
#include "BRepBndLib.hxx"
#include "BRepAdaptor_Surface.hxx"
#include "ShapeUpgrade_ShapeDivideArea.hxx"
#include "ShapeUpgrade_FaceDivideArea.hxx"
#include "ShapeCustom_BSplineRestriction.hxx"
#include "ShapeCustom.hxx"
#include "GeomAbs_Shape.hxx"
#include "ShapeCustom_RestrictionParameters.hxx"
#include "TopOpeBRepBuild_Tools.hxx"

#include "EntityGeometry.h"
#include "DataBase.h"

void ShapeHealing::healSelectedShapes(double tolerance, bool fixSmallEdges, bool fixSmallFaces, bool sewFaces, bool makeSolids)
{
	std::list<ot::EntityInformation> selectedGeometryEntities = application->getSelectedGeometryEntities();
	application->getEntityCache()->prefetchEntities(selectedGeometryEntities);

	// Filter the list of selected shapes for editable ones
	std::list<EntityGeometry *> geometryEntities;
	std::list<EntityBase *> bboxEntities;
	std::list<ot::UID> requiredBreps;
	for (auto shape : selectedGeometryEntities)
	{
		EntityGeometry* geometryEntity = dynamic_cast<EntityGeometry*>(application->modelComponent()->readEntityFromEntityIDandVersion(shape.getID(), shape.getVersion(), application->getClassFactory()));

		if (geometryEntity->getEditable())
		{
			long long brepPrefetchID = geometryEntity->getBrepPrefetchID();

			if (brepPrefetchID >= 0)
			{
				bboxEntities.push_back(geometryEntity);
				geometryEntities.push_back(geometryEntity);
				requiredBreps.push_back(brepPrefetchID);
			}
		}
		else
		{
			delete geometryEntity;
			geometryEntity = nullptr;
		}
	}

	// Cancel the operation if no editable shapes are selected
	if (geometryEntities.empty())
	{
		std::string messages = "____________________________________________________________\n\nNo valid editable shapes selected for healing.\n\n";
		application->uiComponent()->displayMessage(messages);

		return;
	}	
	
	// Now get the information about all the breps
	std::list<ot::EntityInformation> brepEntityInfo;
	application->modelComponent()->getEntityInformation(requiredBreps, brepEntityInfo);

	// Prefetch all the brep entities
	application->getEntityCache()->prefetchEntities(brepEntityInfo);

	// We have some valid shapes, start the healing operation
	ot::LockTypeFlags lockFlags;
	lockFlags.setFlag(ot::LockModelWrite);
	lockFlags.setFlag(ot::LockNavigationWrite);
	lockFlags.setFlag(ot::LockViewWrite);
	lockFlags.setFlag(ot::LockProperties);

	application->uiComponent()->lockUI(lockFlags);
	application->uiComponent()->setProgressInformation("Heal shapes", false);
	application->uiComponent()->setProgress(0);

	std::string messages = "____________________________________________________________\n\nShape healing started.\n\n";

	std::time_t timer = time(nullptr);

	application->modelComponent()->enableMessageQueueing(true);

	auto brepInfo = brepEntityInfo.begin();

	int count = 0;
	for (auto geometryEntity : geometryEntities)
	{
		// Perform the actual shape healing
		EntityBrep *brepEntity = dynamic_cast<EntityBrep*>(application->getEntityCache()->getEntity(brepInfo->getID(), brepInfo->getVersion()));
		if (brepEntity == nullptr)
		{
			brepInfo++;
			continue;
		}

		TopoDS_Shape brep = brepEntity->getBrep();
		healShape(geometryEntity->getName(), brep, tolerance, fixSmallEdges, fixSmallFaces, sewFaces, makeSolids, messages);

		// Store the new brep and facet entity (create new entity IDs)
		geometryEntity->resetBrep();
		geometryEntity->resetFacets();

		geometryEntity->getBrepEntity()->setEntityID(application->modelComponent()->createEntityUID());
		geometryEntity->getFacets()->setEntityID(application->modelComponent()->createEntityUID());

		// Add the new brep entity to the geometry entity
		geometryEntity->setBrep(brep);

		// Facet the geometry entity and store it
		geometryEntity->facetEntity(false);

		geometryEntity->storeBrep();

		ot::UID brepStorageVersion = geometryEntity->getBrepEntity()->getEntityStorageVersion();

		application->getEntityCache()->cacheEntity(geometryEntity->getBrepEntity());
		geometryEntity->detachBrep();

		geometryEntity->storeFacets();

		ot::UID facetsStorageVersion = geometryEntity->getFacets()->getEntityStorageVersion();

		geometryEntity->releaseFacets();

		// Store the information about the entities such that they can be added to the model

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_UpdateGeometryEntity, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, geometryEntity->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID_Brep, (unsigned long long) geometryEntity->getBrepStorageObjectID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID_Facets, (unsigned long long) geometryEntity->getFacetsStorageObjectID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion_Brep, brepStorageVersion, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion_Facets, facetsStorageVersion, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_OverrideGeometry, false, doc.GetAllocator());	// The modified entity was not yet written
		ot::PropertyGridCfg cfg;
		geometryEntity->getProperties().addToConfiguration(nullptr, false, cfg);

		ot::JsonObject cfgObj;
		cfg.addToJsonObject(cfgObj, doc.GetAllocator());

		doc.AddMember(OT_ACTION_PARAM_MODEL_NewProperties, cfgObj, doc.GetAllocator());

		std::string tmp;
		application->modelComponent()->sendMessage(false, doc, tmp);

		delete geometryEntity;
		geometryEntity = nullptr;

		count++;
		application->uiComponent()->setProgress((int) (100 * count / geometryEntities.size()));

		double seconds = difftime(time(nullptr), timer);
		if (seconds >= 2.0)
		{
			// The last message report is more than two seconds ago
			application->uiComponent()->displayMessage(messages);
			messages.clear();

			timer = time(nullptr);
		}

		// Switch to the next brep entity
		brepInfo++;
	}

	messages += "Shape healing completed.\n\n";

	if (!messages.empty())
	{
		application->uiComponent()->displayMessage(messages);
	}

	// Finally, store the new model state
	application->modelComponent()->modelChangeOperationCompleted("heal shapes");
	application->modelComponent()->enableMessageQueueing(false);
	application->uiComponent()->refreshSelection(application->modelComponent()->getCurrentVisualizationModelID());

	application->uiComponent()->setProgress(100);
	application->uiComponent()->closeProgressInformation();

	// Unlock the ui
	application->uiComponent()->unlockUI(lockFlags);
}

void ShapeHealing::healShape(const std::string &name, TopoDS_Shape &myshape, double tolerance, bool fixSmallEdges, bool fixSmallFaces, bool sewFaces, bool makeSolids, std::string &messages)
{
	// These are the cleaning operations which should always be performed
	messages.append("Healing shape: " + name + "\n");

	std::map<std::string, bool> correctionSteps;

	// Fixing de-generated edges
	try {
		Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
		rebuild->Apply(myshape);
		TopExp_Explorer exp;
		bool removedEdge = false;
		for (exp.Init(myshape, TopAbs_EDGE); exp.More(); exp.Next()) {
			TopoDS_Edge edge = TopoDS::Edge(exp.Current());
			if (BRep_Tool::Degenerated(edge))
			{
				removedEdge = true;
				rebuild->Remove(edge);
			}
		}
		myshape = rebuild->Apply(myshape);

		if (removedEdge)
		{
			correctionSteps["removed degenerated edges"] = true;
		}
	}
	catch (...)
	{
		assert(0);
	}

	// Now repair the faces
	try {
		Handle(ShapeFix_Face) sff;
		Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
		rebuild->Apply(myshape);

		Quantity_Color face_colour;
		TopExp_Explorer exp;
		for (exp.Init(myshape, TopAbs_FACE); exp.More(); exp.Next())
		{
			TopoDS_Face face = TopoDS::Face(exp.Current());

			sff = new ShapeFix_Face(face);
			sff->FixAddNaturalBoundMode() = Standard_True;
			sff->FixSmallAreaWireMode() = Standard_True;
			sff->FixPeriodicDegeneratedMode() = Standard_True;
			sff->FixMissingSeamMode() = Standard_True;
			//sff->FixWireTool()->FixRemovePCurveMode() = 1;
			sff->Perform();

			if (sff->Status(ShapeExtend_DONE1) || sff->Status(ShapeExtend_DONE2) ||
				sff->Status(ShapeExtend_DONE3) || sff->Status(ShapeExtend_DONE4) ||
				sff->Status(ShapeExtend_DONE5))
			{
				TopoDS_Face newface = sff->Face();
				rebuild->Replace(face, newface);

				if (sff->Status(ShapeExtend_DONE1)) correctionSteps["fixed some wires"] = true;
				if (sff->Status(ShapeExtend_DONE2))	correctionSteps["fixed orientation of some wires"] = true;
				if (sff->Status(ShapeExtend_DONE3))	correctionSteps["added missing seam to face"] = true;
				if (sff->Status(ShapeExtend_DONE4))	correctionSteps["removed small area wire removed from face"] = true;
				if (sff->Status(ShapeExtend_DONE5))	correctionSteps["added natural bounds to face"] = true;
			}
		}
		myshape = rebuild->Apply(myshape);
	}
	catch (...)
	{
		assert(0);
	}

	// Fixing de-generated edges which might have occurred due to the shape repair
	try {
		Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
		rebuild->Apply(myshape);
		TopExp_Explorer exp;
		bool removedEdge = false;
		for (exp.Init(myshape, TopAbs_EDGE); exp.More(); exp.Next()) {
			TopoDS_Edge edge = TopoDS::Edge(exp.Current());
			if (BRep_Tool::Degenerated(edge))
			{
				removedEdge = true;
				rebuild->Remove(edge);
			}
		}
		myshape = rebuild->Apply(myshape);

		if (removedEdge)
		{
			correctionSteps["removed degenerated edges"] = true;
		}
	} 
	catch (...)
	{
		assert(0);
	}

	// Now try to fix small edges if this option is turned on
	if (fixSmallEdges)
	{
		try
		{
			Handle(ShapeFix_Wire) sfw;
			Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
			rebuild->Apply(myshape);

			TopExp_Explorer exp;
			for (exp.Init(myshape, TopAbs_FACE); exp.More(); exp.Next())
			{
				TopoDS_Face face = TopoDS::Face(exp.Current());

				TopExp_Explorer exp1;
				for (exp1.Init(face, TopAbs_WIRE); exp1.More(); exp1.Next())
				{
					TopoDS_Wire oldwire = TopoDS::Wire(exp1.Current());
					sfw = new ShapeFix_Wire(oldwire, face, tolerance);
					sfw->ModifyTopologyMode() = Standard_True;

					sfw->ClosedWireMode() = Standard_True;

					bool replace = false;
					replace = sfw->FixReorder() || replace;
					replace = sfw->FixConnected() || replace;

					if (sfw->FixSmall(Standard_False, tolerance) &&
						!(sfw->StatusSmall(ShapeExtend_FAIL1) ||
							sfw->StatusSmall(ShapeExtend_FAIL2) ||
							sfw->StatusSmall(ShapeExtend_FAIL3))) {
						replace = true;

						correctionSteps["small edges fixed"] = true;
					}

					replace = sfw->FixEdgeCurves() || replace;
					replace = sfw->FixDegenerated() || replace;
					replace = sfw->FixSelfIntersection() || replace;
					replace = sfw->FixLacking(Standard_True) || replace;
					if (replace) {
						TopoDS_Wire newwire = sfw->Wire();
						rebuild->Replace(oldwire, newwire);
					}
				}
			}
			myshape = rebuild->Apply(myshape);
		}
		catch (...)
		{
			assert(0);
		}

		try {
			Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
			rebuild->Apply(myshape);

			TopExp_Explorer exp;
			for (exp.Init(myshape, TopAbs_EDGE); exp.More(); exp.Next())
			{
				TopoDS_Edge edge = TopoDS::Edge(exp.Current());

				GProp_GProps system;
				BRepGProp::LinearProperties(edge, system);
				if (system.Mass() < tolerance)
				{
					rebuild->Remove(edge);
					correctionSteps["removed degenerated edges"] = true;
				}
			}
			myshape = rebuild->Apply(myshape);
		}
		catch (...)
		{
			assert(0);
		}

		// Fixing de-generated edges which might have occurred due to the shape repair
		try {
			Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
			rebuild->Apply(myshape);
			TopExp_Explorer exp;
			bool removedEdge = false;
			for (exp.Init(myshape, TopAbs_EDGE); exp.More(); exp.Next()) {
				TopoDS_Edge edge = TopoDS::Edge(exp.Current());
				if (BRep_Tool::Degenerated(edge))
				{
					removedEdge = true;
					rebuild->Remove(edge);
				}
			}
			myshape = rebuild->Apply(myshape);

			if (removedEdge)
			{
				correctionSteps["removed degenerated edges"] = true;
			}
		}
		catch (...)
		{
			assert(0);
		}

		// Fixing gaps and small edges in wires
		try {
			Handle(ShapeFix_Wireframe) sfwf = new ShapeFix_Wireframe;
			sfwf->SetPrecision(tolerance);
			sfwf->Load(myshape);
			sfwf->ModeDropSmallEdges() = Standard_True;

			Bnd_Box Box;

			BRepBndLib::Add(myshape, Box);
			double xmin{ 0.0 }, xmax{ 0.0 }, ymin{ 0.0 }, ymax{ 0.0 }, zmin{ 0.0 }, zmax{ 0.0 };
			Box.Get(xmin, ymin, zmin, xmax, ymax, zmax);

			double diag = sqrt((xmax - xmin) * (xmax - xmin) + (ymax - ymin) * (ymax - ymin) + (zmax - zmin) * (zmax - zmin));

			sfwf->SetPrecision(diag);

			if (sfwf->FixWireGaps())
			{
				if (sfwf->StatusWireGaps(ShapeExtend_DONE1)) correctionSteps["fixed some 2D gaps"] = true;
				if (sfwf->StatusWireGaps(ShapeExtend_DONE2)) correctionSteps["fixed some 3D gaps"] = true;
				if (sfwf->StatusWireGaps(ShapeExtend_FAIL1)) correctionSteps["some 2D gaps could not be fixed"] = true;
				if (sfwf->StatusWireGaps(ShapeExtend_FAIL2)) correctionSteps["some 3D gaps could not be fixed"] = true;
			}

			sfwf->SetPrecision(tolerance);

			if (sfwf->FixSmallEdges())
			{
				if (sfwf->StatusSmallEdges(ShapeExtend_DONE1)) correctionSteps["fixed some small edges"] = true;
				if (sfwf->StatusSmallEdges(ShapeExtend_FAIL1)) correctionSteps["some small edges could not be fixed"] = true;
			}

			myshape = sfwf->Shape();
		}
		catch (...)
		{
			assert(0);
		}
	}

	// Fix small faces if this option is turned on
	if (fixSmallFaces)
	{
		try
		{
			Handle(ShapeFix_FixSmallFace) sffsm = new ShapeFix_FixSmallFace();
			sffsm->Init(myshape);
			sffsm->SetPrecision(tolerance);
			sffsm->Perform();

			myshape = sffsm->FixShape();
		}
		catch (...)
		{
			assert(0);
		}
	}

	// Sew faces if this option is turned on
	if (sewFaces)
	{
		try
		{
			BRepOffsetAPI_Sewing sewedObj(tolerance);
			TopExp_Explorer exp;
			for (exp.Init(myshape, TopAbs_FACE); exp.More(); exp.Next()) {
				TopoDS_Face face = TopoDS::Face(exp.Current());
				sewedObj.Add(face);
			}

			sewedObj.Perform();

			if (!sewedObj.SewedShape().IsNull())
			{
				myshape = sewedObj.SewedShape();
			}
		}
		catch (...)
		{
			assert(0);
		}
	}

	// Fixing de-generated edges which might have occurred due to the shape repair
	try {
		Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
		rebuild->Apply(myshape);
		TopExp_Explorer exp;
		bool removedEdge = false;
		for (exp.Init(myshape, TopAbs_EDGE); exp.More(); exp.Next()) {
			TopoDS_Edge edge = TopoDS::Edge(exp.Current());
			if (BRep_Tool::Degenerated(edge))
			{
				removedEdge = true;
				rebuild->Remove(edge);
			}
		}
		myshape = rebuild->Apply(myshape);

		if (removedEdge)
		{
			correctionSteps["removed degenerated edges"] = true;
		}
	}
	catch (...)
	{
		assert(0);
	}

	// Try building solid topologies if this option is turned on
	if (makeSolids)
	{
		try
		{
			BRepBuilderAPI_MakeSolid ms;
			int count = 0;
			TopExp_Explorer exp;
			for (exp.Init(myshape, TopAbs_SHELL); exp.More(); exp.Next()) {
				count++;
				ms.Add(TopoDS::Shell(exp.Current()));
			}

			if (count)
			{
				BRepCheck_Analyzer ba(ms);

				if (ba.IsValid())
				{
					Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape;
					sfs->Init(ms);
					sfs->SetPrecision(tolerance);
					sfs->SetMaxTolerance(tolerance);
					sfs->Perform();
					myshape = sfs->Shape();

					TopExp_Explorer exp;
					for (exp.Init(myshape, TopAbs_SOLID); exp.More(); exp.Next())
					{
						TopoDS_Solid solid = TopoDS::Solid(exp.Current());
						TopoDS_Solid newsolid = solid;
						BRepLib::OrientClosedSolid(newsolid);
						Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
						rebuild->Replace(solid, newsolid);
						TopoDS_Shape newshape = rebuild->Apply(myshape, TopAbs_COMPSOLID);
						myshape = newshape;
					}
				}
			}
		}
		catch (...)
		{
			assert(0);
		}
	}

	// Now we analyze the faces and provide an information in case of complex surfaces

	//for (TopExp_Explorer aExpFace(myshape, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
	//{
	//	TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());
	//	TopAbs_Orientation faceOrientation = aFace.Orientation();

	//	BRepAdaptor_Surface surface(aFace, Standard_False);

	//	std::string surfaceTypes[] = { "plane", "cylinder", "cone", "sphere", "torus", "bezier surface", "BSpline surface", "surface of revolution",
	//								   "surface of extrusion", "offset surface", "other surface" };
	//	std::string surfaceType;
	//	if (surface.GetType() < 0 || surface.GetType() > 10)
	//	{
	//		surfaceType = "unknown";
	//	}
	//	else
	//	{
	//		surfaceType = surfaceTypes[surface.GetType()];
	//	}

	//	if (surface.GetType() == GeomAbs_BSplineSurface)
	//	{
	//		int unodes = surface.Surface().NbUKnots();
	//		int vnodes = surface.Surface().NbVKnots();
	//		int udegree = surface.Surface().UDegree();
	//		int vdegree = surface.Surface().VDegree();
	//		std::cout << "";
	//	}

	//	std::cout << "surface type: " + surfaceType << std::endl;
	//}

	// Let's check whether the shape is a topologically correct solid (each edge has exactly two faces)

	TopTools_IndexedDataMapOfShapeListOfShape edgeFaceMap;
	TopExp::MapShapesAndAncestors(myshape, TopAbs_EDGE, TopAbs_FACE, edgeFaceMap);

	for (TopExp_Explorer aExpEdge(myshape, TopAbs_EDGE); aExpEdge.More(); aExpEdge.Next())
	{
		TopTools_ListOfShape faces = edgeFaceMap.FindFromKey(aExpEdge.Current());

		if (faces.Size() != 2)
		{
			correctionSteps["the shape is not a solid, try healing with a larger tolerance"] = true;
		}
	}

	// Display the output
	if (correctionSteps.empty()) correctionSteps["shape is valid"];

	for (auto step : correctionSteps)
	{
		messages.append("\t-" + step.first + "\n");
	}
}

