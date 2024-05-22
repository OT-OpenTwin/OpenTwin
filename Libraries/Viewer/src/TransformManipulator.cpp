#include "stdafx.h"
#include "TransformManipulator.h"

#include "Viewer.h"
#include "Notifier.h"
#include "SceneNodeBase.h"
#include "SceneNodeGeometry.h"
#include "HandleArrow.h"
#include "HandleWheel.h"
#include "Model.h"

#include "OTCore/Logger.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyDouble.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#define PROP_Group_Rotation "Rotation"
#define PROP_Group_Translation "Translation"

#define PROP_Item_TranslateX "Translate X"
#define PROP_Item_TranslateY "Translate Y"
#define PROP_Item_TranslateZ "Translate Z"

#define PROP_Item_AxisX "Axis X"
#define PROP_Item_AxisY "Axis Y"
#define PROP_Item_AxisZ "Axis Z"
#define PROP_Item_Angle "Angle (deg.)"

TransformManipulator::TransformManipulator(Viewer *viewer, std::list<SceneNodeBase *> objects)
{
	assert(!objects.empty());
	assert(viewer != nullptr);

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

	// Create the configuration
	ot::PropertyGridCfg cfg;

	{
		ot::PropertyGroup* gTranslation = new ot::PropertyGroup(PROP_Group_Translation);

		ot::PropertyDouble* iTranslateX = new ot::PropertyDouble(PROP_Item_TranslateX, lastPropertyOffset.x());
		ot::PropertyDouble* iTranslateY = new ot::PropertyDouble(PROP_Item_TranslateY, lastPropertyOffset.y());
		ot::PropertyDouble* iTranslateZ = new ot::PropertyDouble(PROP_Item_TranslateZ, lastPropertyOffset.z());

		gTranslation->addProperty(iTranslateX);
		gTranslation->addProperty(iTranslateY);
		gTranslation->addProperty(iTranslateZ);
		cfg.addRootGroup(gTranslation);
	}
	
	{
		ot::PropertyGroup* gRotation = new ot::PropertyGroup(PROP_Group_Rotation);

		ot::PropertyDouble* iRotationX = new ot::PropertyDouble(PROP_Item_AxisX, lastPropertyAxis.x());
		ot::PropertyDouble* iRotationY = new ot::PropertyDouble(PROP_Item_AxisY, lastPropertyAxis.y());
		ot::PropertyDouble* iRotationZ = new ot::PropertyDouble(PROP_Item_AxisZ, lastPropertyAxis.z());
		ot::PropertyDouble* iAngle = new ot::PropertyDouble(PROP_Item_Angle, lastPropertyAngle);

		gRotation->addProperty(iRotationX);
		gRotation->addProperty(iRotationY);
		gRotation->addProperty(iRotationZ);
		gRotation->addProperty(iAngle);
		cfg.addRootGroup(gRotation);
	}

	assert(viewer3D->getModel() != nullptr);
	viewer3D->getModel()->fillPropertyGrid(cfg);
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

	if (offset.x() != lastPropertyOffset.x()) updateSetting(PROP_Group_Translation, PROP_Item_TranslateX, offset.x());
	if (offset.y() != lastPropertyOffset.y()) updateSetting(PROP_Group_Translation, PROP_Item_TranslateY, offset.y());
	if (offset.z() != lastPropertyOffset.z()) updateSetting(PROP_Group_Translation, PROP_Item_TranslateZ, offset.z());

	if (axis.x() != lastPropertyAxis.x()) updateSetting(PROP_Group_Rotation, PROP_Item_AxisX, axis.x());
	if (axis.y() != lastPropertyAxis.y()) updateSetting(PROP_Group_Rotation, PROP_Item_AxisY, axis.y());
	if (axis.z() != lastPropertyAxis.z()) updateSetting(PROP_Group_Rotation, PROP_Item_AxisZ, axis.z());
	if (angle    != lastPropertyAngle)    updateSetting(PROP_Group_Rotation, PROP_Item_Angle, angle);

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

void TransformManipulator::updateSetting(const std::string& _groupName, const std::string& _itemName, double value)
{
	assert(viewer3D->getModel() != nullptr);

	if (fabs(value) < 1e-6) value = 0.0;  // Trim small values to 0
	viewer3D->getModel()->setDoublePropertyGridValue(_groupName, _itemName, value);
}

bool TransformManipulator::propertyGridValueChanged(const std::string& _groupName, const std::string& _itemName)
{
	osg::Vec3d offset = lastPropertyOffset;

	if (_groupName == PROP_Group_Rotation) {
		if (_itemName == PROP_Item_AxisX) {
			lastPropertyAxis = osg::Vec3d(viewer3D->getModel()->getDoublePropertyGridValue(_groupName, _itemName), lastPropertyAxis.y(), lastPropertyAxis.z());
		}
		else if (_itemName == PROP_Item_AxisY) {
			lastPropertyAxis = osg::Vec3d(lastPropertyAxis.x(), viewer3D->getModel()->getDoublePropertyGridValue(_groupName, _itemName), lastPropertyAxis.z());
		}
		else if (_itemName == PROP_Item_AxisZ) {
			lastPropertyAxis = osg::Vec3d(lastPropertyAxis.x(), lastPropertyAxis.y(), viewer3D->getModel()->getDoublePropertyGridValue(_groupName, _itemName));
		}
		else if (_itemName == PROP_Item_Angle) {
			lastPropertyAngle = viewer3D->getModel()->getDoublePropertyGridValue(_groupName, _itemName);
		}
		else {
			OT_LOG_E("Unknown property \"" + _itemName + "\"");
		}
	}
	else if (_groupName == PROP_Group_Translation) {
		if (_itemName == PROP_Item_TranslateX) {
			offset.set(viewer3D->getModel()->getDoublePropertyGridValue(_groupName, _itemName), offset.y(), offset.z());
		}
		else if (_itemName == PROP_Item_TranslateY) {
			offset.set(offset.x(), viewer3D->getModel()->getDoublePropertyGridValue(_groupName, _itemName), offset.z());
		}
		else if (_itemName == PROP_Item_TranslateZ) {
			offset.set(offset.x(), offset.y(), viewer3D->getModel()->getDoublePropertyGridValue(_groupName, _itemName));
		}
		else {
			OT_LOG_E("Unknown property \"" + _itemName + "\"");
		}
	}
	else {
		OT_LOG_E("Unknown property group \"" + _groupName + "\"");
		return false;
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


