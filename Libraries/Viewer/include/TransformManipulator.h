// @otlicense

#pragma once

#include "HandlerBase.h"
#include "ManipulatorBase.h"

class Viewer;
class SceneNodeBase;
class HandleArrow;
class HandleWheel;

#include <osg/Vec3d>

#include "rapidjson/document.h"

class TransformManipulator : public HandlerNotifier, public ManipulatorBase
{
public:
	TransformManipulator() = delete;
	TransformManipulator(Viewer *viewer, std::list<SceneNodeBase *> objects);

	virtual ~TransformManipulator();

	virtual void handlerInteraction(HandlerBase *handler) override;

	virtual void cancelOperation(void) override;
	virtual void performOperation(void) override;

	virtual bool propertyGridValueChanged(const ot::Property* _property) override;

private:
	void getBoundingSphere(osg::Vec3d &center, double &radius, std::list<SceneNodeBase *> objects);
	void updateHandlerPositions(void);
	void storeTransformations(void);
	void applyObjectTransformations(void);
	void setPropertyGrid(void);
	void updatePropertyGrid(void);
	void addSetting(rapidjson::Document &jsonDoc, const std::string &group, const std::string &name, double value);
	void updateSetting(const std::string& _groupName, const std::string& _itemName, double value);

	osg::Vec3d m_sphereCenter;
	osg::Vec3d m_initialSphereCenter;
	double m_sphereRadius;
	HandleArrow* m_arrowHandlers[6];
	HandleWheel* m_wheelHandlers[3];
	osg::Vec3d m_handlerPosition[6];
	Viewer* m_viewer3D;
	std::list<SceneNodeBase *> m_transformedObjects;
	std::map<SceneNodeBase *, osg::Matrix> m_initialObjectTransform;
	osg::Vec3d m_lastPropertyOffset;
	osg::Vec3d m_lastPropertyAxis;
	double m_lastPropertyAngle;
	double m_rotationDegX;
	double m_rotationDegY;
	double m_rotationDegZ;
	osg::Matrix m_totalRotation;
	osg::Matrix m_workingPlaneRotation;
};

