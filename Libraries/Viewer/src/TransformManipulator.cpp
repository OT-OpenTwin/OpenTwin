#include "stdafx.h"
#include "TransformManipulator.h"

#include "Viewer.h"
#include "Notifier.h"
#include "SceneNodeBase.h"
#include "SceneNodeGeometry.h"
#include "HandleArrow.h"
#include "HandleWheel.h"
#include "Model.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

TransformManipulator::TransformManipulator(Viewer *viewer, std::list<SceneNodeBase *> objects)
{
	assert(!objects.empty());
	assert(viewer != nullptr);

	propertyOffsetX = 0;
	propertyOffsetY = 0;
	propertyOffsetZ = 0;

	propertyAxisX = 0;
	propertyAxisY = 0;
	propertyAxisZ = 0;
	propertyAngle = 0;

	viewer3D = viewer;
	transformedObjects = objects;

	// Store the transformation of all the objects
	storeTransformations();

	// Get the current working plane transformation
	osg::Matrix workingPlaneTransform = viewer3D->getWorkingPlaneTransform();

	workingPlaneRotation = osg::Matrix(workingPlaneTransform.getRotate());
	//osg::Matrix workingPlaneRotationT;
	//workingPlaneRotationT.transpose(workingPlaneRotation);

	// Get the bounding sphere of the objects
	getBoundingSphere(sphereCenter, sphereRadius, objects);

	initialSphereCenter = sphereCenter;

	double colorX[] = { 1.0, 0.0, 0.0, 1.0 };
	double colorY[] = { 0.0, 1.0, 0.0, 1.0 };
	double colorZ[] = { 0.0, 0.0, 1.0, 1.0 };

	double arrowSize          = 0.2   * sphereRadius;
	double arrowHandlerRadius = 0.025 * sphereRadius;
	double wheelHandlerRadius = 0.02  * sphereRadius;

	// Create the handles for the six coordinate directions
	osg::Vec3d vectorX(1.0, 0.0, 0.0);
	osg::Vec3d vectorY(0.0, 1.0, 0.0);
	osg::Vec3d vectorZ(0.0, 0.0, 1.0);

	vectorX = workingPlaneRotation * vectorX;
	vectorY = workingPlaneRotation * vectorY;
	vectorZ = workingPlaneRotation * vectorZ;

	handlerPosition[0] = sphereCenter - vectorX * sphereRadius;
	handlerPosition[1] = sphereCenter + vectorX * sphereRadius;
	handlerPosition[2] = sphereCenter - vectorY * sphereRadius;
	handlerPosition[3] = sphereCenter + vectorY * sphereRadius;
	handlerPosition[4] = sphereCenter - vectorZ * sphereRadius;
	handlerPosition[5] = sphereCenter + vectorZ * sphereRadius;

	arrowHandlers[0] = new HandleArrow(handlerPosition[0], -vectorX, colorX, arrowSize, arrowHandlerRadius);  // -> -x Direction
	arrowHandlers[1] = new HandleArrow(handlerPosition[1],  vectorX, colorX, arrowSize, arrowHandlerRadius);  // -> +x Direction
	arrowHandlers[2] = new HandleArrow(handlerPosition[2], -vectorY, colorY, arrowSize, arrowHandlerRadius);  // -> -y Direction
	arrowHandlers[3] = new HandleArrow(handlerPosition[3],  vectorY, colorY, arrowSize, arrowHandlerRadius);  // -> +y Direction
	arrowHandlers[4] = new HandleArrow(handlerPosition[4], -vectorZ, colorZ, arrowSize, arrowHandlerRadius);  // -> -z Direction
	arrowHandlers[5] = new HandleArrow(handlerPosition[5],  vectorZ, colorZ, arrowSize, arrowHandlerRadius);  // -> +z Direction

	for (int i = 0; i < 6; i++)
	{
		viewer3D->addHandler(arrowHandlers[i]);
		arrowHandlers[i]->addNotifier(this);
	}

	wheelHandlers[0] = new HandleWheel(sphereCenter, vectorX, sphereRadius, wheelHandlerRadius, colorX);  // -> x Normal
	wheelHandlers[1] = new HandleWheel(sphereCenter, vectorY, sphereRadius, wheelHandlerRadius, colorY);  // -> y Normal
	wheelHandlers[2] = new HandleWheel(sphereCenter, vectorZ, sphereRadius, wheelHandlerRadius, colorZ);  // -> z Normal

	for (int i = 0; i < 3; i++)
	{
		viewer3D->addHandler(wheelHandlers[i]);
		wheelHandlers[i]->addNotifier(this);
	}

	rotationDegX = 0.0;
	rotationDegY = 0.0;
	rotationDegZ = 0.0;

	// Set the property grid
	setPropertyGrid();

	// Lock the ui
	viewer3D->getModel()->lockSelectionAndModification(true);
}

TransformManipulator::~TransformManipulator()
{
	// Detach the handlers
	for (int i = 0; i < 6; i++)
	{
		viewer3D->removeHandler(arrowHandlers[i]);
	
		delete arrowHandlers[i];
		arrowHandlers[i] = nullptr;
	}

	for (int i = 0; i < 3; i++)
	{
		viewer3D->removeHandler(wheelHandlers[i]);

		delete wheelHandlers[i];
		wheelHandlers[i] = nullptr;
	}
}

void TransformManipulator::cancelOperation(void)
{
	// Here we need to reset the transformations of the nodes
	for (auto item : transformedObjects)
	{
		SceneNodeGeometry *geometryItem = dynamic_cast<SceneNodeGeometry *>(item);
		if (geometryItem != nullptr)
		{
			geometryItem->applyTransform(initialObjectTransform[item]);
		}
	}

	assert(viewer3D->getModel() != nullptr);

	// Unlock the ui
	viewer3D->getModel()->lockSelectionAndModification(false);

	// Refresh the selection (will also restore the original property grid
	viewer3D->getModel()->refreshSelection();
}

void TransformManipulator::performOperation(void)
{
	assert(viewer3D->getModel() != nullptr);

	// Unlock the ui
	viewer3D->getModel()->lockSelectionAndModification(false);

	// Now we just need to send the property grid settings
	ot::JsonDocument doc;
	doc.AddMember("Translate X", lastPropertyOffset.x(), doc.GetAllocator());
	doc.AddMember("Translate Y", lastPropertyOffset.y(), doc.GetAllocator());
	doc.AddMember("Translate Z", lastPropertyOffset.z(), doc.GetAllocator());

	doc.AddMember("Axis X", lastPropertyAxis.x(), doc.GetAllocator());
	doc.AddMember("Axis Y", lastPropertyAxis.y(), doc.GetAllocator());
	doc.AddMember("Axis Z", lastPropertyAxis.z(), doc.GetAllocator());

	doc.AddMember("Angle", lastPropertyAngle, doc.GetAllocator());
	
	std::string selectionInfo = doc.toJson();

	// Send the selection message to the model
	getNotifier()->entitiesSelected(messageReplyTo, messageSelectionAction, selectionInfo, messageOptionNames, messageOptionValues);
}

void TransformManipulator::storeTransformations(void)
{
	for (auto item : transformedObjects)
	{
		osg::Matrix transformation;
		SceneNodeGeometry *geometryItem = dynamic_cast<SceneNodeGeometry *>(item);
		if (geometryItem != nullptr)
		{
			transformation = geometryItem->getParentTransformation();
		}

		initialObjectTransform[item] = transformation;
	}
}

void TransformManipulator::getBoundingSphere(osg::Vec3d &center, double &radius, std::list<SceneNodeBase *> objects)
{
	osg::BoundingSphere boundingSphere;

	bool first = true;
	for (auto item : objects)
	{
		if (first)
		{
			boundingSphere = item->getShapeNode()->getBound();
			first = false;
		}
		else
		{
			boundingSphere.expandBy(item->getShapeNode()->getBound());
		}
	}
	
	center = boundingSphere.center();
	radius = boundingSphere.radius();
}

void TransformManipulator::updateHandlerPositions(void)
{
	osg::Vec3d vectorX(1.0, 0.0, 0.0);
	osg::Vec3d vectorY(0.0, 1.0, 0.0);
	osg::Vec3d vectorZ(0.0, 0.0, 1.0);

	vectorX = workingPlaneRotation * vectorX;
	vectorY = workingPlaneRotation * vectorY;
	vectorZ = workingPlaneRotation * vectorZ;

	handlerPosition[0] = sphereCenter - vectorX * sphereRadius;
	handlerPosition[1] = sphereCenter + vectorX * sphereRadius;
	handlerPosition[2] = sphereCenter - vectorY * sphereRadius;
	handlerPosition[3] = sphereCenter + vectorY * sphereRadius;
	handlerPosition[4] = sphereCenter - vectorZ * sphereRadius;
	handlerPosition[5] = sphereCenter + vectorZ * sphereRadius;

	for (int i = 0; i < 6; i++)
	{
		arrowHandlers[i]->setPosition(handlerPosition[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		wheelHandlers[i]->setPosition(sphereCenter);
	}
}

void TransformManipulator::handlerInteraction(HandlerBase *handler)
{
	bool applyTransform = false;

	osg::Vec3d vectorX(1.0, 0.0, 0.0);
	osg::Vec3d vectorY(0.0, 1.0, 0.0);
	osg::Vec3d vectorZ(0.0, 0.0, 1.0);

	vectorX = workingPlaneRotation * vectorX;
	vectorY = workingPlaneRotation * vectorY;
	vectorZ = workingPlaneRotation * vectorZ;

	for (int i = 0; i < 6; i++)
	{
		if (arrowHandlers[i] == handler)
		{
			// Determine the offset of the handler position 
			osg::Vec3 offset = arrowHandlers[i]->getPosition() - handlerPosition[i];

			// Determine the new sphere center
			sphereCenter = sphereCenter + offset;

			// Assign new positions to all handlers
			updateHandlerPositions();

			applyTransform = true;
			break;
		}
	}

	if (!applyTransform && wheelHandlers[0] == handler)
	{
		// Determine the offset of the handler position 
		double deltaRotX = wheelHandlers[0]->getRotationAngleDeg() - rotationDegX;
		osg::Matrix deltaRotate;
		deltaRotate.makeRotate(deltaRotX * M_PI / 180.0, vectorX);
		totalRotation = totalRotation * deltaRotate;
		rotationDegX = wheelHandlers[0]->getRotationAngleDeg();

		applyTransform = true;
	}

	if (!applyTransform && wheelHandlers[1] == handler)
	{
		// Determine the offset of the handler position 
		double deltaRotY = wheelHandlers[1]->getRotationAngleDeg() - rotationDegY;
		osg::Matrix deltaRotate;
		deltaRotate.makeRotate(deltaRotY * M_PI / 180.0, vectorY);
		totalRotation = totalRotation * deltaRotate;
		rotationDegY = wheelHandlers[1]->getRotationAngleDeg();

		applyTransform = true;
	}

	if (!applyTransform && wheelHandlers[2] == handler)
	{
		// Determine the offset of the handler position 
		double deltaRotZ = wheelHandlers[2]->getRotationAngleDeg() - rotationDegZ;
		osg::Matrix deltaRotate;
		deltaRotate.makeRotate(deltaRotZ * M_PI / 180.0, vectorZ);
		totalRotation = totalRotation * deltaRotate;
		rotationDegZ = wheelHandlers[2]->getRotationAngleDeg();

		applyTransform = true;
	}

	if (applyTransform)
	{
		// Transform entities
		applyObjectTransformations();

		// Update the property grid
		updatePropertyGrid();
	}
}

void TransformManipulator::applyObjectTransformations(void)
{
	// Determine the current transformation (consider translation)
	osg::Vec3d offset = sphereCenter - initialSphereCenter;

	osg::Matrix translation;
	translation.makeTranslate(offset);

	// Now consider the rotation
	osg::Matrix translationToOrigin;
	translationToOrigin.makeTranslate(-initialSphereCenter);

	osg::Matrix translationFromOrigin;
	translationFromOrigin.makeTranslate(initialSphereCenter);

	osg::Matrix rotation = translationToOrigin * totalRotation * translationFromOrigin;

	// Combine the matrices
	osg::Matrix transformation;
	transformation = rotation * translation;

	// Now apply the transformation to each object
	// Here we need to reset the transformations of the nodes
	for (auto item : transformedObjects)
	{
		SceneNodeGeometry *geometryItem = dynamic_cast<SceneNodeGeometry *>(item);
		if (geometryItem != nullptr)
		{
			osg::Matrix newTransformation = initialObjectTransform[item] * transformation;
			geometryItem->applyTransform(newTransformation);
		}
	}
}

void TransformManipulator::setPropertyGrid(void)
{
	lastPropertyOffset = sphereCenter - initialSphereCenter;

	osg::Quat totQuat = totalRotation.getRotate();
	totQuat.getRotate(lastPropertyAngle, lastPropertyAxis);
	lastPropertyAngle *= 180.0 / M_PI;

	// Create the document
	rapidjson::Document jsonDoc;
	jsonDoc.SetObject();	

	// fill the document data
	addSetting(jsonDoc, "Translation", "Translate X", lastPropertyOffset.x());
	addSetting(jsonDoc, "Translation", "Translate Y", lastPropertyOffset.y());
	addSetting(jsonDoc, "Translation", "Translate Z", lastPropertyOffset.z());

	addSetting(jsonDoc, "Rotation", "Axis X", lastPropertyAxis.x());
	addSetting(jsonDoc, "Rotation", "Axis Y", lastPropertyAxis.y());
	addSetting(jsonDoc, "Rotation", "Axis Z", lastPropertyAxis.z());
	addSetting(jsonDoc, "Rotation", "Angle (deg.)", lastPropertyAngle);

	// Write the settings to the property grid

	rapidjson::StringBuffer buffer;
	buffer.Clear();

	// Setup the Writer
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	jsonDoc.Accept(writer);

	// Create the output string
	std::string settings = buffer.GetString();

	assert(viewer3D->getModel() != nullptr);
	viewer3D->getModel()->fillPropertyGrid(settings);

	// Now get the item IDs for accessing the properties
	propertyOffsetX = viewer3D->getModel()->findPropertyItem("Translate X");
	propertyOffsetY = viewer3D->getModel()->findPropertyItem("Translate Y");
	propertyOffsetZ = viewer3D->getModel()->findPropertyItem("Translate Z");

	propertyAxisX = viewer3D->getModel()->findPropertyItem("Axis X");
	propertyAxisY = viewer3D->getModel()->findPropertyItem("Axis Y");
	propertyAxisZ = viewer3D->getModel()->findPropertyItem("Axis Z");
	propertyAngle = viewer3D->getModel()->findPropertyItem("Angle (deg.)");
}

void TransformManipulator::updatePropertyGrid(void)
{
	// We first build the current transformation matrix and apply the working plane rotation
	osg::Matrix workingPlaneRotationI;
	workingPlaneRotationI = workingPlaneRotationI.inverse(workingPlaneRotation);

	osg::Matrix currentTransformT;
	currentTransformT.makeTranslate(sphereCenter - initialSphereCenter);

	osg::Matrix currentTransformR = totalRotation;
	osg::Matrix currentTransform = currentTransformR * currentTransformT;

	currentTransform = currentTransform * workingPlaneRotation;

	// From the rotated transformation matrix in the working plane coordinates, we can now get the properties for the translation and rotation settings
	osg::Vec3d offset = currentTransform.getTrans();

	//osg::Quat totQuat = totalRotation.getRotate();
	osg::Matrix rotateProperty = workingPlaneRotationI * osg::Matrix(currentTransform.getRotate());
	osg::Quat totQuat = rotateProperty.getRotate();
	osg::Vec3d axis;
	double angle = 0.0;
	totQuat.getRotate(angle, axis);
	angle *= 180.0 / M_PI;

	if (fabs(angle) < 1e-6)
	{
		axis = osg::Vec3d(0.0, 0.0, 0.0);
		angle = 0.0;
	}

	if (offset.x() != lastPropertyOffset.x()) updateSetting(propertyOffsetX, offset.x());
	if (offset.y() != lastPropertyOffset.y()) updateSetting(propertyOffsetY, offset.y());
	if (offset.z() != lastPropertyOffset.z()) updateSetting(propertyOffsetZ, offset.z());

	if (axis.x() != lastPropertyAxis.x()) updateSetting(propertyAxisX, axis.x());
	if (axis.y() != lastPropertyAxis.y()) updateSetting(propertyAxisY, axis.y());
	if (axis.z() != lastPropertyAxis.z()) updateSetting(propertyAxisZ, axis.z());
	if (angle    != lastPropertyAngle)    updateSetting(propertyAngle, angle);

	lastPropertyOffset = offset;
	lastPropertyAxis = axis;
	lastPropertyAngle = angle;
}

void TransformManipulator::addSetting(rapidjson::Document &jsonDoc, const std::string &group, const std::string &name, double value)
{
	rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

	rapidjson::Value container(rapidjson::kObjectType);

	rapidjson::Value jsonName(name.c_str(), allocator);
	rapidjson::Value jsonType("double", allocator);
	rapidjson::Value jsonMixed(rapidjson::kNumberType);
	rapidjson::Value jsonReadOnly(rapidjson::kNumberType);
	rapidjson::Value jsonVisible(rapidjson::kNumberType);
	rapidjson::Value jsonGroup(rapidjson::kStringType);
	rapidjson::Value jsonValue(rapidjson::kNumberType);

	jsonMixed.SetBool(false);
	jsonReadOnly.SetBool(false);
	jsonVisible.SetBool(true);
	jsonGroup.SetString(group.c_str(), allocator);
	jsonValue.SetDouble(value);

	container.AddMember("Type", jsonType, allocator);
	container.AddMember("MultipleValues", jsonMixed, allocator);
	container.AddMember("ReadOnly", jsonReadOnly, allocator);
	container.AddMember("Visible", jsonVisible, allocator);
	container.AddMember("Group", jsonGroup, allocator);
	container.AddMember("Value", jsonValue, allocator);

	//rapidjson::Value::StringRefType jsonName(name.c_str());

	jsonDoc.AddMember(jsonName, container, allocator);
}

void TransformManipulator::updateSetting(int itemID, double value)
{
	assert(viewer3D->getModel() != nullptr);

	if (fabs(value) < 1e-6) value = 0.0;  // Trim small values to 0
	viewer3D->getModel()->setDoublePropertyGridValue(itemID, value);
}

bool TransformManipulator::propertyGridValueChanged(int itemID)
{
	osg::Vec3d offset = lastPropertyOffset;

	if (itemID == propertyOffsetX)
	{
		offset.set(viewer3D->getModel()->getDoublePropertyGridValue(itemID), offset.y(), offset.z());
	}
	else if (itemID == propertyOffsetY)
	{
		offset.set(offset.x(), viewer3D->getModel()->getDoublePropertyGridValue(itemID), offset.z());
	}
	else if (itemID == propertyOffsetZ)
	{
		offset.set(offset.x(), offset.y(), viewer3D->getModel()->getDoublePropertyGridValue(itemID));
	}
	else if (itemID == propertyAxisX)
	{
		lastPropertyAxis = osg::Vec3d(viewer3D->getModel()->getDoublePropertyGridValue(itemID), lastPropertyAxis.y(), lastPropertyAxis.z());
	}
	else if (itemID == propertyAxisY)
	{
		lastPropertyAxis = osg::Vec3d(lastPropertyAxis.x(), viewer3D->getModel()->getDoublePropertyGridValue(itemID), lastPropertyAxis.z());
	}
	else if (itemID == propertyAxisZ)
	{
		lastPropertyAxis = osg::Vec3d(lastPropertyAxis.x(), lastPropertyAxis.y(), viewer3D->getModel()->getDoublePropertyGridValue(itemID));
	}
	else if (itemID == propertyAngle)
	{
		lastPropertyAngle = viewer3D->getModel()->getDoublePropertyGridValue(itemID);
	}

	lastPropertyOffset = offset;

	// Update the transformation
	osg::Matrix propertyRotate;
	propertyRotate.makeRotate(lastPropertyAngle * M_PI / 180.0, lastPropertyAxis);
	propertyRotate = workingPlaneRotation * propertyRotate;

	osg::Matrix propertyTranslate;
	propertyTranslate.makeTranslate(offset);

	osg::Matrix currentTransform = propertyRotate * propertyTranslate;

	osg::Matrix workingPlaneRotationI;
	workingPlaneRotationI = workingPlaneRotationI.inverse(workingPlaneRotation);

	currentTransform =   currentTransform * workingPlaneRotationI;

	offset = currentTransform.getTrans();
	totalRotation = osg::Matrix(currentTransform.getRotate());



//	totalRotation.makeRotate(lastPropertyAngle * M_PI / 180.0, lastPropertyAxis);
//	totalRotation = totalRotation * workingPlaneRotation;

	sphereCenter = initialSphereCenter + offset;

	// Assign new positions to all handlers
	updateHandlerPositions();

	// Transform entities
	applyObjectTransformations();

	// Refresh the scene
	viewer3D->refresh();

	return true;  // We handle the property grid change
}


