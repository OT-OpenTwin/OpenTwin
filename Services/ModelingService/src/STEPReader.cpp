#include "STEPReader.h"
#include "EntityGeometry.h"
#include "GeometryOperations.h"
#include "DataBase.h"
#include "ShapeHealing.h"
#include "Application.h"

#include <OpenTwinCommunication/Msg.h>
#include <OpenTwinFoundation/ModelComponent.h>
#include <OpenTwinFoundation/uiComponent.h>

#include "XSControl_WorkSession.hxx"
#include "XSControl_TransferReader.hxx"
#include "StepRepr_RepresentationItem.hxx"
#include "XCAFApp_Application.hxx"
#include "XCAFDoc_DocumentTool.hxx"
#include "XCAFDoc_ShapeTool.hxx"
#include "XCAFDoc_ColorTool.hxx"
#include "TDF_Label.hxx"
#include "TDocStd_Document.hxx"
#include "TDataStd_Name.hxx"
#include "Quantity_Color.hxx"
#include "STEPCAFControl_Reader.hxx"
#include "TopoDS_Iterator.hxx"
#include "BRepBuilderAPI_Transform.hxx"
#include "ShapeFix_Shape.hxx"
#include "ShapeFix_Wireframe.hxx"
#include "ShapeFix_FixSmallFace.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Solid.hxx"
#include "TopoDS.hxx"
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
#include "ShapeCustom_RestrictionParameters.hxx"
#include "ShapeCustom.hxx"

#include <iostream>
#include <filesystem>

std::string STEPReader::materialsFolder;
ot::UID STEPReader::materialsFolderID = 0;

void STEPReader::importSTEPFile(const std::string &fileName, bool removeFile, const std::string &originalName)
{
	std::thread workerThread(&STEPReader::importSTEPFileWorker, this, fileName, removeFile, originalName);
	workerThread.detach();
}

void STEPReader::importSTEPFileWorker(const std::string &fileName, bool removeFile, const std::string &originalName)
{
	entityList.clear();

	// Read the STEP file
	ot::Flags<ot::ui::lockType> lockFlags;
	lockFlags.setFlag(ot::ui::lockType::tlModelWrite);
	lockFlags.setFlag(ot::ui::lockType::tlNavigationWrite);
	lockFlags.setFlag(ot::ui::lockType::tlViewWrite);
	lockFlags.setFlag(ot::ui::lockType::tlProperties);

	application->uiComponent()->lockUI(lockFlags);
	application->uiComponent()->setProgressInformation("Read STEP file", false);
	application->uiComponent()->setProgress(0);

	application->uiComponent()->displayMessage("____________________________________________________________\n\nReading STEP file\n\n");

	std::string rootName = determineUniqueRootName(originalName);

	// The step file import reads the entities into a flat list and assigns the entity names. The entities are not yet added to the model. All entities are EntityGeometry

	// Enable write caching to database and message queueing
	DataBase::GetDataBase()->queueWriting(true);
	application->modelComponent()->enableMessageQueueing(true);

	std::string messages;
	readStepFile(fileName, rootName, removeFile, messages);

	// Disable write caching to database (this will also flush all pending writes)
	DataBase::GetDataBase()->queueWriting(false);
	application->uiComponent()->setProgress(95);

	std::list<ot::UID> topologyEntityIDList;
	std::list<ot::UID> topologyEntityVersionList;
	std::list<bool> topologyEntityForceVisible;
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	for (EntityGeometry *entity : entityList)
	{
		topologyEntityIDList.push_back(entity->getEntityID());
		topologyEntityVersionList.push_back(entity->getEntityStorageVersion());
		topologyEntityForceVisible.push_back(false);

		dataEntityIDList.push_back(entity->getBrepEntity()->getEntityID());
		dataEntityVersionList.push_back(entity->getBrepEntity()->getEntityStorageVersion());
		dataEntityParentList.push_back(entity->getEntityID());

		dataEntityIDList.push_back(entity->getFacets()->getEntityID());
		dataEntityVersionList.push_back(entity->getFacets()->getEntityStorageVersion());
		dataEntityParentList.push_back(entity->getEntityID());

		delete entity;
	}

	application->modelComponent()->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
												      dataEntityIDList, dataEntityVersionList, dataEntityParentList, "import STEP file" );

	messages = std::to_string(entityList.size()) + " shape(s) imported.\n\n" + messages;
	application->uiComponent()->displayMessage(messages);

	// Disable message queueing. This will send all messages.
	application->modelComponent()->enableMessageQueueing(false);

	// Refresh the view
	application->uiComponent()->refreshAllViews(application->modelComponent()->getCurrentVisualizationModelID());

	application->uiComponent()->setProgress(100);
	application->uiComponent()->closeProgressInformation();
	application->uiComponent()->unlockUI(lockFlags);
}

void STEPReader::analyzeGeometry(EntityGeometry *entityGeom, std::string &messages)
{
	const double maxRatioForComplexSplines = 3.5;

	TopoDS_Shape shape = entityGeom->getBrep(); 

	double maxRatio = 0;

	for (TopExp_Explorer aExpFace(shape, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());
		BRepAdaptor_Surface surface(aFace, Standard_False);

		if (surface.GetType() == GeomAbs_BSplineSurface)
		{
			double maxEdgeLength = 0.0;

			for (TopExp_Explorer aExpEdge(aFace, TopAbs_EDGE); aExpEdge.More(); aExpEdge.Next())
			{
				GProp_GProps gprops;
				BRepGProp::LinearProperties(aExpEdge.Current(), gprops);
				double edgeLength = gprops.Mass();
				if (edgeLength > maxEdgeLength) maxEdgeLength = edgeLength;
			}

			// Get bounding box of face
			Bnd_Box Box;
			BRepBndLib::Add(aFace, Box);
			double boxDiag = sqrt(Box.SquareExtent());

			// Check the ratio between the length of the longest edge and the diagonal of the bounding box (helical  faces have a large ratio here)
			double ratioLength = maxEdgeLength / boxDiag;
			maxRatio = std::max(maxRatio, ratioLength);
		}
	}

	if (maxRatio >= maxRatioForComplexSplines)
	{
		messages += "INFORMATION: " + entityGeom->getName() + " has faces with a large number of spline nodes (" + std::to_string(maxRatio) + ").\n"
			"These faces are automatically simplified by splitting them into multiple less complex faces. It might be necessary to perform a healing operation on this shape.\n\n";

		simplifyComplexSplines(entityGeom, maxRatioForComplexSplines);
	}
}

void STEPReader::simplifyComplexSplines(EntityGeometry *entityGeom, double maxRatioForComplexSplines)
{
	TopoDS_Shape shape = entityGeom->getBrep();

	//		Handle_ShapeCustom_RestrictionParameters restrictPar = new ShapeCustom_RestrictionParameters;
	//		double localtol = tolerance / 10.0;
	//		TopoDS_Shape newShape = ShapeCustom::BSplineRestriction(shape, localtol, localtol, 3, 5, GeomAbs_C0, GeomAbs_C0, true, true, restrictPar);
	//		shape = newShape;

	bool shapeModified = false;

	for (int iloop = 1; iloop <= 10; iloop++)
	{
		bool faceReplaced = false;

		for (TopExp_Explorer aExpFace(shape, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
		{
			TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());
			BRepAdaptor_Surface surface(aFace, Standard_False);

			if (surface.GetType() == GeomAbs_BSplineSurface)
			{
				double maxEdgeLength = 0.0;

				for (TopExp_Explorer aExpEdge(aFace, TopAbs_EDGE); aExpEdge.More(); aExpEdge.Next())
				{
					GProp_GProps gprops;
					BRepGProp::LinearProperties(aExpEdge.Current(), gprops);
					double edgeLength = gprops.Mass();
					if (edgeLength > maxEdgeLength) maxEdgeLength = edgeLength;
				}

				// Get bounding box of face
				Bnd_Box Box;
				BRepBndLib::Add(aFace, Box);
				double boxDiag = sqrt(Box.SquareExtent());

				// Check the ratio between the length of the longest edge and the diagonal of the bounding box (helical  faces have a large ratio here)
				double ratioLength = maxEdgeLength / boxDiag;

				if (ratioLength >= maxRatioForComplexSplines)
				{
					GProp_GProps gprops;
					BRepGProp::SurfaceProperties(aFace, gprops);
					double area = gprops.Mass();

					//Handle_ShapeCustom_RestrictionParameters restrictPar = new ShapeCustom_RestrictionParameters;
					//double localtol = 1e-3;
					//TopoDS_Shape newFace = ShapeCustom::BSplineRestriction(aFace, localtol, localtol, 3, 5, GeomAbs_C0, GeomAbs_C0, true, true, restrictPar);

					Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
					rebuild->Apply(shape);

//					ShapeUpgrade_ShapeDivideArea divider(newFace);
					ShapeUpgrade_ShapeDivideArea divider(aFace);
					divider.MaxArea() = area / ratioLength;
					divider.Perform();
					TopoDS_Shape faceShape = divider.Result();

					rebuild->Replace(aExpFace.Current(), faceShape);
					shape = rebuild->Apply(shape);
					shapeModified = true;

					faceReplaced = true;
				}
			}
		}

		if (!faceReplaced) break;
		assert(iloop < 10); // If this assertion fires, we havce reached the maximum number of refinement operations
	}

	if (shapeModified)
	{
		ShapeHealing healer(application);
		std::string messages;
		healer.healShape(entityGeom->getName(), shape, 1e-9, true, true, true, true, messages);

		entityGeom->setBrep(shape);
	}
}

std::string STEPReader::getNameFromLabel(const TDF_Label &label)
{
	std::string name;

	Handle(TDataStd_Name) nameAttribute;

	if (label.FindAttribute(TDataStd_Name::GetID(), nameAttribute))
	{
		TCollection_ExtendedString extstr = nameAttribute->Get();

		char* str = new char[extstr.LengthOfCString() + 1];
		extstr.ToUTF8CString(str);
		name = str;
		delete[] str;
	}

	return name;
}

std::string STEPReader::getEntityName(const std::string &prefix, const TDF_Label &label, const TopoDS_Shape &theShape, STEPCAFControl_Reader *aReader)
{
	std::string name;

	// Try to get name from representation item first

	const Handle(XSControl_WorkSession)& theSession = aReader->Reader().WS();
	const Handle(XSControl_TransferReader)& aTransferReader = theSession->TransferReader();

	Handle(Standard_Transient) anEntity = aTransferReader->EntityFromShapeResult(theShape, 1);

	if (anEntity.IsNull()) {
		// as just mapped
		anEntity = aTransferReader->EntityFromShapeResult(theShape, -1);
	}

	if (anEntity.IsNull()) {
		// as anything
		anEntity = aTransferReader->EntityFromShapeResult(theShape, 4);
	}

	if (!anEntity.IsNull()) 
	{
		Handle(StepRepr_RepresentationItem) aReprItem = Handle(StepRepr_RepresentationItem)::DownCast(anEntity);;

		if (aReprItem.IsNull()) {
			std::cout << "Error: STEPReader::ReadAttributes():\nStepRepr_RepresentationItem Is NULL" << std::endl;
		}
		else
		{
			name = aReprItem->Name()->ToCString();
		}
	}

	if (name == "")
	{
		// If this does not work, get name from label
		name = getNameFromLabel(label);
	}

	bool addPrefix = true;

	// When importing STEP files which were generated by CST Studio Suite the last name separator is a | character. This needs to be 
	// replaced by / in order to create the entire tree structure. In this case, the name already contains the entire path
	// and therefore no prefix needs to be added
	for (int c = 0; c < name.length(); c++)
	{
		if (name[c] == '|')
		{
			name[c] = '/';
			addPrefix = false;
		}
	}

	// Finally, add the prefix if needed
	if (addPrefix && !prefix.empty())
	{
		name = prefix + name;
	}

	// Ensure a proper name in case that no name is provided
	if (name == "") name = "import";

	return name;
}

Quantity_Color STEPReader::readColor(const TDF_Label &label, const TopoDS_Shape &shape)
{
	Quantity_Color aColor;

	if (colorTool->IsSet(shape, XCAFDoc_ColorGen)) {
		colorTool->GetColor(shape, XCAFDoc_ColorGen, aColor);
	}
	else if (colorTool->IsSet(shape, XCAFDoc_ColorSurf)) {
		colorTool->GetColor(shape, XCAFDoc_ColorSurf, aColor);
	}
	else if (colorTool->IsSet(shape, XCAFDoc_ColorCurv)) {
		colorTool->GetColor(shape, XCAFDoc_ColorCurv, aColor);
	}
	else if (colorTool->IsSet(label, XCAFDoc_ColorGen)) {
		colorTool->GetColor(label, XCAFDoc_ColorGen, aColor);
	}
	else if (colorTool->IsSet(label, XCAFDoc_ColorSurf)) {
		colorTool->GetColor(label, XCAFDoc_ColorSurf, aColor);
	}
	else if (colorTool->IsSet(label, XCAFDoc_ColorCurv)) {
		colorTool->GetColor(label, XCAFDoc_ColorCurv, aColor);
	}
	else
	{
		TopoDS_Iterator it;
		for (it.Initialize(shape); it.More(); it.Next())
		{
			if (colorTool->IsSet(it.Value(), XCAFDoc_ColorGen)) {
				colorTool->GetColor(it.Value(), XCAFDoc_ColorGen, aColor);
			}
			else if (colorTool->IsSet(it.Value(), XCAFDoc_ColorSurf)) {
				colorTool->GetColor(it.Value(), XCAFDoc_ColorSurf, aColor);
			}
			else if (colorTool->IsSet(it.Value(), XCAFDoc_ColorCurv)) {
				colorTool->GetColor(it.Value(), XCAFDoc_ColorCurv, aColor);
			}
		}
	}

	return aColor;
}

void STEPReader::processNode(const TDF_Label &itemLabel, std::string prefix, STEPCAFControl_Reader *reader, TopLoc_Location aLocation, const std::string &rootName, 
							 std::map<std::string, bool> &existingEntityNames, std::string &messages, int &shapesIndex, int numberOfShapes, bool buildShapes)
{
	std::string itemName = getNameFromLabel(itemLabel);

	if (shapeTool->IsAssembly(itemLabel))
	{
		TDF_LabelSequence childLabels;
		if (shapeTool->GetComponents(itemLabel, childLabels))
		{
			prefix = prefix + itemName + "/";

			for (int i = 1; i <= childLabels.Length(); i++)
			{
				TDF_Label childLabel = childLabels.Value(i);

				processNode(childLabel, prefix, reader, aLocation, rootName, existingEntityNames, messages, shapesIndex, numberOfShapes, buildShapes);
			}
		}
	}
	else if (shapeTool->IsReference(itemLabel))
	{
		TopLoc_Location loc = shapeTool->GetLocation(itemLabel);

		aLocation = aLocation.Multiplied(loc);

		TDF_Label referredLabel;
		shapeTool->GetReferredShape(itemLabel, referredLabel);

		processNode(referredLabel, prefix, reader, aLocation, rootName, existingEntityNames, messages, shapesIndex, numberOfShapes, buildShapes);
	}
	else
	{
		TopoDS_Shape itemShape;
		if (shapeTool->GetShape(itemLabel, itemShape))
		{
			if (buildShapes)
			{
				Quantity_Color aColor = readColor(itemLabel, itemShape);
				std::string name = getEntityName(prefix, itemLabel, itemShape, reader);

				if (!aLocation.IsIdentity())
				{
					gp_Trsf myTransf = aLocation.Transformation();

					BRepBuilderAPI_Transform transform(itemShape, myTransf);
					itemShape = transform.ModifiedShape(itemShape);
				}

				int count = 1;

				// It may be that the body is a compound. In this case, we need to split the compound.
				std::list<TopoDS_Shape> shapes;
				splitCompoundShapes(itemShape, shapes);

				if (materialsFolder.empty())
				{
					// The materials folder information has not yet been retrieved. We get the information about the entity from the model
					materialsFolder = "Materials";

					std::list<std::string> entityList{ materialsFolder };
					std::list<ot::EntityInformation> entityInfo;

					application->modelComponent()->getEntityInformation(entityList, entityInfo);

					assert(entityInfo.size() == 1);
					assert(entityInfo.front().getName() == materialsFolder);

					materialsFolderID = entityInfo.front().getID();
				}

				for (auto itemShape : shapes)
				{
					// Make name unique
					std::string thisName = rootName + "/" + name;

					std::string nameBase = thisName;
					while (existingEntityNames.find(thisName) != existingEntityNames.end())
					{
						thisName = nameBase + "_" + std::to_string(count);
						count++;
					}

					BRepTools::Clean(itemShape);

					double red = Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.Red());
					double green = Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.Green());
					double blue = Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.Blue());

					int colorR = (int)(red * 255 + 0.5);
					int colorG = (int)(green * 255 + 0.5);
					int colorB = (int)(blue * 255 + 0.5);

					// Store entity
					ot::UID entityID = application->modelComponent()->createEntityUID();
					ot::UID brepID = application->modelComponent()->createEntityUID();
					ot::UID facetsID = application->modelComponent()->createEntityUID();

					EntityGeometry *entityGeom = new EntityGeometry(entityID, nullptr, nullptr, nullptr, nullptr, serviceName);
					entityGeom->setName(thisName);
					entityGeom->setEditable(true);
					entityGeom->setBrep(itemShape);

					entityGeom->createProperties(colorR, colorG, colorB, materialsFolder, materialsFolderID);

					analyzeGeometry(entityGeom, messages);

					entityGeom->getBrepEntity()->setEntityID(brepID);
					entityGeom->getFacets()->setEntityID(facetsID);

					entityGeom->facetEntity(false);
					entityGeom->StoreToDataBase();

					entityList.push_back(entityGeom);
					existingEntityNames[thisName] = true;
				}

				// Set progress
				int percent = 20 + (int)(75.0 * shapesIndex / numberOfShapes);
				application->uiComponent()->setProgress(percent);
			}

			shapesIndex++;
		}
		else
		{
			assert(0);
		}
	}
}

void STEPReader::splitCompoundShapes(TopoDS_Shape &itemShape, std::list<TopoDS_Shape> &shapeList)
{
	TopExp_Explorer exp;
	for (exp.Init(itemShape, TopAbs_SOLID); exp.More(); exp.Next())
	{
		TopoDS_Shape currentShape = exp.Current();
		shapeList.push_back(currentShape);
	}
}

void STEPReader::readStepFile(std::string fileName, const std::string &rootName, bool removeFile, std::string &messages)
{

	// Set up the reader and the document
	STEPCAFControl_Reader reader;
	reader.SetNameMode(true);
	reader.SetColorMode(true);

	reader.ReadFile((Standard_CString)fileName.c_str());
	application->uiComponent()->setProgress(5);

	Handle(TDocStd_Document) doc;
	Handle(XCAFApp_Application) anApp = XCAFApp_Application::GetApplication();
	anApp->NewDocument("MDTV-XCAF", doc);

	// Transfer file to a document
	try
	{
		if (!reader.Transfer(doc)) {
			assert(0);
		}
	}
	catch (...)
	{
	}

	application->uiComponent()->setProgress(20);
	
	if (removeFile)
	{
		// Remove the file
		std::remove(fileName.c_str());
	}

	shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
	colorTool = XCAFDoc_DocumentTool::ColorTool(doc->Main());

	// Get a list of assembly root labels
	TDF_LabelSequence rootLabels;
	shapeTool->GetFreeShapes(rootLabels);

	TopLoc_Location aLocation;
	aLocation.Identity();

	std::map<std::string, bool> existingEntityNames;

	// First, count the number of shapes
	int numberOfShapes = 0;

	for (int i = 1; i <= rootLabels.Length(); i++)
	{
		TDF_Label rootLabel = rootLabels.Value(i);

		processNode(rootLabel, "", &reader, aLocation, rootName, existingEntityNames, messages, numberOfShapes, 0, false);
	}

	// Now build the shapes

	int shapesIndex = 0;
	for (int i = 1; i <= rootLabels.Length(); i++)
	{
		TDF_Label rootLabel = rootLabels.Value(i);

		processNode(rootLabel, "", &reader, aLocation, rootName, existingEntityNames, messages, shapesIndex, numberOfShapes, true);
	}
}

std::string STEPReader::determineUniqueRootName(const std::string &fileName)
{
	// Get the base name for the step import folder
	std::string baseName = "Geometry/" + std::filesystem::path(fileName).stem().string();

	// First get a list of all folder items of the Geometries folder
	std::list<std::string> geometryItems = application->modelComponent()->getListOfFolderItems("Geometry");

	// Create a unique name for the new solver item
	std::string rootName = baseName;

	int count = 1;
	while (std::find(geometryItems.begin(), geometryItems.end(), rootName) != geometryItems.end())
	{
		rootName = baseName + "_" + std::to_string(count);
		count++;
	}

	return rootName;
}
