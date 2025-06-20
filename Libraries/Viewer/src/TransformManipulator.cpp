#include "stdafx.h"
#include "TransformManipulator.h"

#include "Viewer.h"
#include "FrontendAPI.h"
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

	m_viewer3D = viewer;
	m_transformedObjects = objects;

	// Store the transformation of all the objects
	storeTransformations();

	// Get the current working plane transformation
	osg::Matrix workingPlaneTransform = m_viewer3D->getWorkingPlaneTransform();

	m_workingPlaneRotation = osg::Matrix(workingPlaneTransform.getRotate());
	//osg::Matrix workingPlaneRotationT;
	//workingPlaneRotationT.transpose(workingPlaneRotation);

	// Get the bounding sphere of the objects
	getBoundingSphere(m_sphereCenter, m_sphereRadius, objects);

	m_initialSphereCenter = m_sphereCenter;

	double colorX[] = { 1.0, 0.0, 0.0, 1.0 };
	double colorY[] = { 0.0, 1.0, 0.0, 1.0 };
	double colorZ[] = { 0.0, 0.0, 1.0, 1.0 };

	double arrowSize          = 0.2   * m_sphereRadius;
	double arrowHandlerRadius = 0.025 * m_sphereRadius;
	double wheelHandlerRadius = 0.02  * m_sphereRadius;

	// Create the handles for the six coordinate directions
	osg::Vec3d vectorX(1.0, 0.0, 0.0);
	osg::Vec3d vectorY(0.0, 1.0, 0.0);
	osg::Vec3d vectorZ(0.0, 0.0, 1.0);

	vectorX = m_workingPlaneRotation * vectorX;
	vectorY = m_workingPlaneRotation * vectorY;
	vectorZ = m_workingPlaneRotation * vectorZ;

	m_handlerPosition[0] = m_sphereCenter - vectorX * m_sphereRadius;
	m_handlerPosition[1] = m_sphereCenter + vectorX * m_sphereRadius;
	m_handlerPosition[2] = m_sphereCenter - vectorY * m_sphereRadius;
	m_handlerPosition[3] = m_sphereCenter + vectorY * m_sphereRadius;
	m_handlerPosition[4] = m_sphereCenter - vectorZ * m_sphereRadius;
	m_handlerPosition[5] = m_sphereCenter + vectorZ * m_sphereRadius;

	m_arrowHandlers[0] = new HandleArrow(m_handlerPosition[0], -vectorX, colorX, arrowSize, arrowHandlerRadius);  // -> -x Direction
	m_arrowHandlers[1] = new HandleArrow(m_handlerPosition[1],  vectorX, colorX, arrowSize, arrowHandlerRadius);  // -> +x Direction
	m_arrowHandlers[2] = new HandleArrow(m_handlerPosition[2], -vectorY, colorY, arrowSize, arrowHandlerRadius);  // -> -y Direction
	m_arrowHandlers[3] = new HandleArrow(m_handlerPosition[3],  vectorY, colorY, arrowSize, arrowHandlerRadius);  // -> +y Direction
	m_arrowHandlers[4] = new HandleArrow(m_handlerPosition[4], -vectorZ, colorZ, arrowSize, arrowHandlerRadius);  // -> -z Direction
	m_arrowHandlers[5] = new HandleArrow(m_handlerPosition[5],  vectorZ, colorZ, arrowSize, arrowHandlerRadius);  // -> +z Direction

	for (int i = 0; i < 6; i++)
	{
		m_viewer3D->addHandler(m_arrowHandlers[i]);
		m_arrowHandlers[i]->addNotifier(this);
	}

	m_wheelHandlers[0] = new HandleWheel(m_sphereCenter, vectorX, m_sphereRadius, wheelHandlerRadius, colorX);  // -> x Normal
	m_wheelHandlers[1] = new HandleWheel(m_sphereCenter, vectorY, m_sphereRadius, wheelHandlerRadius, colorY);  // -> y Normal
	m_wheelHandlers[2] = new HandleWheel(m_sphereCenter, vectorZ, m_sphereRadius, wheelHandlerRadius, colorZ);  // -> z Normal

	for (int i = 0; i < 3; i++)
	{
		m_viewer3D->addHandler(m_wheelHandlers[i]);
		m_wheelHandlers[i]->addNotifier(this);
	}

	m_rotationDegX = 0.0;
	m_rotationDegY = 0.0;
	m_rotationDegZ = 0.0;

	// Set the property grid
	setPropertyGrid();

	// Lock the ui
	m_viewer3D->getModel()->lockSelectionAndModification(true);
}

TransformManipulator::~TransformManipulator()
{
	// Detach the handlers
	for (int i = 0; i < 6; i++)
	{
		m_viewer3D->removeHandler(m_arrowHandlers[i]);
	
		delete m_arrowHandlers[i];
		m_arrowHandlers[i] = nullptr;
	}

	for (int i = 0; i < 3; i++)
	{
		m_viewer3D->removeHandler(m_wheelHandlers[i]);

		delete m_wheelHandlers[i];
		m_wheelHandlers[i] = nullptr;
	}

	// Reset modal property grid
	OTAssertNullptr(m_viewer3D->getModel());
	m_viewer3D->getModel()->clearModalPropertyGrid();
}

void TransformManipulator::cancelOperation(void)
{
	// Here we need to reset the transformations of the nodes
	for (auto item : m_transformedObjects)
	{
		SceneNodeGeometry *geometryItem = dynamic_cast<SceneNodeGeometry *>(item);
		if (geometryItem != nullptr)
		{
			geometryItem->applyTransform(m_initialObjectTransform[item]);
		}
	}

	assert(m_viewer3D->getModel() != nullptr);

	// Unlock the ui
	m_viewer3D->getModel()->lockSelectionAndModification(false);

	// Refresh the selection (will also restore the original property grid
	m_viewer3D->getModel()->refreshSelection();
}

void TransformManipulator::performOperation(void)
{
	assert(m_viewer3D->getModel() != nullptr);

	// Unlock the ui
	m_viewer3D->getModel()->lockSelectionAndModification(false);

	// Now we just need to send the property grid settings
	ot::JsonDocument doc;
	doc.AddMember("Translate X", m_lastPropertyOffset.x(), doc.GetAllocator());
	doc.AddMember("Translate Y", m_lastPropertyOffset.y(), doc.GetAllocator());
	doc.AddMember("Translate Z", m_lastPropertyOffset.z(), doc.GetAllocator());

	doc.AddMember("Axis X", m_lastPropertyAxis.x(), doc.GetAllocator());
	doc.AddMember("Axis Y", m_lastPropertyAxis.y(), doc.GetAllocator());
	doc.AddMember("Axis Z", m_lastPropertyAxis.z(), doc.GetAllocator());

	doc.AddMember("Angle", m_lastPropertyAngle, doc.GetAllocator());
	
	std::string selectionInfo = doc.toJson();

	// Send the selection message to the model
	FrontendAPI::instance()->entitiesSelected(messageReplyTo, messageSelectionAction, selectionInfo, messageOptionNames, messageOptionValues);
}

void TransformManipulator::storeTransformations(void)
{
	for (auto item : m_transformedObjects)
	{
		osg::Matrix transformation;
		SceneNodeGeometry *geometryItem = dynamic_cast<SceneNodeGeometry *>(item);
		if (geometryItem != nullptr)
		{
			transformation = geometryItem->getParentTransformation();
		}

		m_initialObjectTransform[item] = transformation;
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

	vectorX = m_workingPlaneRotation * vectorX;
	vectorY = m_workingPlaneRotation * vectorY;
	vectorZ = m_workingPlaneRotation * vectorZ;

	m_handlerPosition[0] = m_sphereCenter - vectorX * m_sphereRadius;
	m_handlerPosition[1] = m_sphereCenter + vectorX * m_sphereRadius;
	m_handlerPosition[2] = m_sphereCenter - vectorY * m_sphereRadius;
	m_handlerPosition[3] = m_sphereCenter + vectorY * m_sphereRadius;
	m_handlerPosition[4] = m_sphereCenter - vectorZ * m_sphereRadius;
	m_handlerPosition[5] = m_sphereCenter + vectorZ * m_sphereRadius;

	for (int i = 0; i < 6; i++)
	{
		m_arrowHandlers[i]->setPosition(m_handlerPosition[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		m_wheelHandlers[i]->setPosition(m_sphereCenter);
	}
}

void TransformManipulator::handlerInteraction(HandlerBase *handler)
{
	bool applyTransform = false;

	osg::Vec3d vectorX(1.0, 0.0, 0.0);
	osg::Vec3d vectorY(0.0, 1.0, 0.0);
	osg::Vec3d vectorZ(0.0, 0.0, 1.0);

	vectorX = m_workingPlaneRotation * vectorX;
	vectorY = m_workingPlaneRotation * vectorY;
	vectorZ = m_workingPlaneRotation * vectorZ;

	for (int i = 0; i < 6; i++)
	{
		if (m_arrowHandlers[i] == handler)
		{
			// Determine the offset of the handler position 
			osg::Vec3 offset = m_arrowHandlers[i]->getPosition() - m_handlerPosition[i];

			// Determine the new sphere center
			m_sphereCenter = m_sphereCenter + offset;

			// Assign new positions to all handlers
			updateHandlerPositions();

			applyTransform = true;
			break;
		}
	}

	if (!applyTransform && m_wheelHandlers[0] == handler)
	{
		// Determine the offset of the handler position 
		double deltaRotX = m_wheelHandlers[0]->getRotationAngleDeg() - m_rotationDegX;
		osg::Matrix deltaRotate;
		deltaRotate.makeRotate(deltaRotX * M_PI / 180.0, vectorX);
		m_totalRotation = m_totalRotation * deltaRotate;
		m_rotationDegX = m_wheelHandlers[0]->getRotationAngleDeg();

		applyTransform = true;
	}

	if (!applyTransform && m_wheelHandlers[1] == handler)
	{
		// Determine the offset of the handler position 
		double deltaRotY = m_wheelHandlers[1]->getRotationAngleDeg() - m_rotationDegY;
		osg::Matrix deltaRotate;
		deltaRotate.makeRotate(deltaRotY * M_PI / 180.0, vectorY);
		m_totalRotation = m_totalRotation * deltaRotate;
		m_rotationDegY = m_wheelHandlers[1]->getRotationAngleDeg();

		applyTransform = true;
	}

	if (!applyTransform && m_wheelHandlers[2] == handler)
	{
		// Determine the offset of the handler position 
		double deltaRotZ = m_wheelHandlers[2]->getRotationAngleDeg() - m_rotationDegZ;
		osg::Matrix deltaRotate;
		deltaRotate.makeRotate(deltaRotZ * M_PI / 180.0, vectorZ);
		m_totalRotation = m_totalRotation * deltaRotate;
		m_rotationDegZ = m_wheelHandlers[2]->getRotationAngleDeg();

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
	osg::Vec3d offset = m_sphereCenter - m_initialSphereCenter;

	osg::Matrix translation;
	translation.makeTranslate(offset);

	// Now consider the rotation
	osg::Matrix translationToOrigin;
	translationToOrigin.makeTranslate(-m_initialSphereCenter);

	osg::Matrix translationFromOrigin;
	translationFromOrigin.makeTranslate(m_initialSphereCenter);

	osg::Matrix rotation = translationToOrigin * m_totalRotation * translationFromOrigin;

	// Combine the matrices
	osg::Matrix transformation;
	transformation = rotation * translation;

	// Now apply the transformation to each object
	// Here we need to reset the transformations of the nodes
	for (auto item : m_transformedObjects)
	{
		SceneNodeGeometry *geometryItem = dynamic_cast<SceneNodeGeometry *>(item);
		if (geometryItem != nullptr)
		{
			osg::Matrix newTransformation = m_initialObjectTransform[item] * transformation;
			geometryItem->applyTransform(newTransformation);
		}
	}
}

void TransformManipulator::setPropertyGrid(void)
{
	m_lastPropertyOffset = m_sphereCenter - m_initialSphereCenter;

	osg::Quat totQuat = m_totalRotation.getRotate();
	totQuat.getRotate(m_lastPropertyAngle, m_lastPropertyAxis);
	m_lastPropertyAngle *= 180.0 / M_PI;

	// Create the configuration
	ot::PropertyGridCfg cfg;

	{
		ot::PropertyGroup* gTranslation = new ot::PropertyGroup(PROP_Group_Translation);

		ot::PropertyDouble* iTranslateX = new ot::PropertyDouble(PROP_Item_TranslateX, m_lastPropertyOffset.x());
		ot::PropertyDouble* iTranslateY = new ot::PropertyDouble(PROP_Item_TranslateY, m_lastPropertyOffset.y());
		ot::PropertyDouble* iTranslateZ = new ot::PropertyDouble(PROP_Item_TranslateZ, m_lastPropertyOffset.z());

		gTranslation->addProperty(iTranslateX);
		gTranslation->addProperty(iTranslateY);
		gTranslation->addProperty(iTranslateZ);
		cfg.addRootGroup(gTranslation);
	}
	
	{
		ot::PropertyGroup* gRotation = new ot::PropertyGroup(PROP_Group_Rotation);

		ot::PropertyDouble* iRotationX = new ot::PropertyDouble(PROP_Item_AxisX, m_lastPropertyAxis.x());
		ot::PropertyDouble* iRotationY = new ot::PropertyDouble(PROP_Item_AxisY, m_lastPropertyAxis.y());
		ot::PropertyDouble* iRotationZ = new ot::PropertyDouble(PROP_Item_AxisZ, m_lastPropertyAxis.z());
		ot::PropertyDouble* iAngle = new ot::PropertyDouble(PROP_Item_Angle, m_lastPropertyAngle);

		gRotation->addProperty(iRotationX);
		gRotation->addProperty(iRotationY);
		gRotation->addProperty(iRotationZ);
		gRotation->addProperty(iAngle);
		cfg.addRootGroup(gRotation);
	}

	cfg.setIsModal(true);

	OTAssertNullptr(m_viewer3D->getModel());
	m_viewer3D->getModel()->fillPropertyGrid(cfg);
}

void TransformManipulator::updatePropertyGrid(void)
{
	// We first build the current transformation matrix and apply the working plane rotation
	osg::Matrix workingPlaneRotationI;
	workingPlaneRotationI = workingPlaneRotationI.inverse(m_workingPlaneRotation);

	osg::Matrix currentTransformT;
	currentTransformT.makeTranslate(m_sphereCenter - m_initialSphereCenter);

	osg::Matrix currentTransformR = m_totalRotation;
	osg::Matrix currentTransform = currentTransformR * currentTransformT;

	currentTransform = currentTransform * m_workingPlaneRotation;

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

	if (offset.x() != m_lastPropertyOffset.x()) updateSetting(PROP_Group_Translation, PROP_Item_TranslateX, offset.x());
	if (offset.y() != m_lastPropertyOffset.y()) updateSetting(PROP_Group_Translation, PROP_Item_TranslateY, offset.y());
	if (offset.z() != m_lastPropertyOffset.z()) updateSetting(PROP_Group_Translation, PROP_Item_TranslateZ, offset.z());

	if (axis.x() != m_lastPropertyAxis.x()) updateSetting(PROP_Group_Rotation, PROP_Item_AxisX, axis.x());
	if (axis.y() != m_lastPropertyAxis.y()) updateSetting(PROP_Group_Rotation, PROP_Item_AxisY, axis.y());
	if (axis.z() != m_lastPropertyAxis.z()) updateSetting(PROP_Group_Rotation, PROP_Item_AxisZ, axis.z());
	if (angle    != m_lastPropertyAngle)    updateSetting(PROP_Group_Rotation, PROP_Item_Angle, angle);

	m_lastPropertyOffset = offset;
	m_lastPropertyAxis = axis;
	m_lastPropertyAngle = angle;
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
	assert(m_viewer3D->getModel() != nullptr);

	if (fabs(value) < 1e-6) value = 0.0;  // Trim small values to 0
	m_viewer3D->getModel()->setDoublePropertyGridValue(_groupName, _itemName, value);
}

bool TransformManipulator::propertyGridValueChanged(const ot::Property* _property)
{
	osg::Vec3d offset = m_lastPropertyOffset;

	ot::PropertyGroup* propertyGroup = _property->getParentGroup();
	OTAssertNullptr(propertyGroup);

	if (propertyGroup->getName() == PROP_Group_Rotation) {
		if (_property->getPropertyName() == PROP_Item_AxisX) {
			const ot::PropertyDouble* actualProperty = dynamic_cast<const ot::PropertyDouble*>(_property);
			if (!actualProperty) {
				OT_LOG_EA("Item cast failed");
				return false;
			}
			m_lastPropertyAxis = osg::Vec3d(actualProperty->getValue(), m_lastPropertyAxis.y(), m_lastPropertyAxis.z());
		}
		else if (_property->getPropertyName() == PROP_Item_AxisY) {
			const ot::PropertyDouble* actualProperty = dynamic_cast<const ot::PropertyDouble*>(_property);
			if (!actualProperty) {
				OT_LOG_EA("Item cast failed");
				return false;
			}
			m_lastPropertyAxis = osg::Vec3d(m_lastPropertyAxis.x(), actualProperty->getValue(), m_lastPropertyAxis.z());
		}
		else if (_property->getPropertyName() == PROP_Item_AxisZ) {
			const ot::PropertyDouble* actualProperty = dynamic_cast<const ot::PropertyDouble*>(_property);
			if (!actualProperty) {
				OT_LOG_EA("Item cast failed");
				return false;
			}
			m_lastPropertyAxis = osg::Vec3d(m_lastPropertyAxis.x(), m_lastPropertyAxis.y(), actualProperty->getValue());
		}
		else if (_property->getPropertyName() == PROP_Item_Angle) {
			const ot::PropertyDouble* actualProperty = dynamic_cast<const ot::PropertyDouble*>(_property);
			if (!actualProperty) {
				OT_LOG_EA("Item cast failed");
				return false;
			}
			m_lastPropertyAngle = actualProperty->getValue();
		}
		else {
			OT_LOG_E("Unknown property \"" + _property->getPropertyName() + "\"");
		}
	}
	else if (propertyGroup->getName() == PROP_Group_Translation) {
		if (_property->getPropertyName() == PROP_Item_TranslateX) {
			const ot::PropertyDouble* actualProperty = dynamic_cast<const ot::PropertyDouble*>(_property);
			if (!actualProperty) {
				OT_LOG_EA("Item cast failed");
				return false;
			}
			offset.set(actualProperty->getValue(), offset.y(), offset.z());
		}
		else if (_property->getPropertyName() == PROP_Item_TranslateY) {
			const ot::PropertyDouble* actualProperty = dynamic_cast<const ot::PropertyDouble*>(_property);
			if (!actualProperty) {
				OT_LOG_EA("Item cast failed");
				return false;
			}
			offset.set(offset.x(), actualProperty->getValue(), offset.z());
		}
		else if (_property->getPropertyName() == PROP_Item_TranslateZ) {
			const ot::PropertyDouble* actualProperty = dynamic_cast<const ot::PropertyDouble*>(_property);
			if (!actualProperty) {
				OT_LOG_EA("Item cast failed");
				return false;
			}
			offset.set(offset.x(), offset.y(), actualProperty->getValue());
		}
		else {
			OT_LOG_E("Unknown property \"" + _property->getPropertyName() + "\"");
		}
	}
	else {
		OT_LOG_E("Unknown property group \"" + propertyGroup->getName() + "\"");
		return false;
	}

	m_lastPropertyOffset = offset;

	// Update the transformation
	osg::Matrix propertyRotate;
	propertyRotate.makeRotate(m_lastPropertyAngle * M_PI / 180.0, m_lastPropertyAxis);
	propertyRotate = m_workingPlaneRotation * propertyRotate;

	osg::Matrix propertyTranslate;
	propertyTranslate.makeTranslate(offset);

	osg::Matrix currentTransform = propertyRotate * propertyTranslate;

	osg::Matrix workingPlaneRotationI;
	workingPlaneRotationI = workingPlaneRotationI.inverse(m_workingPlaneRotation);

	currentTransform =   currentTransform * workingPlaneRotationI;

	offset = currentTransform.getTrans();
	m_totalRotation = osg::Matrix(currentTransform.getRotate());



//	totalRotation.makeRotate(lastPropertyAngle * M_PI / 180.0, lastPropertyAxis);
//	totalRotation = totalRotation * workingPlaneRotation;

	m_sphereCenter = m_initialSphereCenter + offset;

	// Assign new positions to all handlers
	updateHandlerPositions();

	// Transform entities
	applyObjectTransformations();

	// Refresh the scene
	m_viewer3D->refresh();

	return true;  // We handle the property grid change
}


