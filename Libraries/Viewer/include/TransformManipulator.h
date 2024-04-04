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

	virtual bool propertyGridValueChanged(const std::string& _groupName, const std::string& _itemName) override;

private:
	void getBoundingSphere(osg::Vec3d &center, double &radius, std::list<SceneNodeBase *> objects);
	void updateHandlerPositions(void);
	void storeTransformations(void);
	void applyObjectTransformations(void);
	void setPropertyGrid(void);
	void updatePropertyGrid(void);
	void addSetting(rapidjson::Document &jsonDoc, const std::string &group, const std::string &name, double value);
	void updateSetting(const std::string& _groupName, const std::string& _itemName, double value);

	osg::Vec3d sphereCenter;
	osg::Vec3d initialSphereCenter;
	double sphereRadius;
	HandleArrow *arrowHandlers[6];
	HandleWheel *wheelHandlers[3];
	osg::Vec3d handlerPosition[6];
	Viewer *viewer3D;
	std::list<SceneNodeBase *> transformedObjects;
	std::map<SceneNodeBase *, osg::Matrix> initialObjectTransform;
	osg::Vec3d lastPropertyOffset;
	osg::Vec3d lastPropertyAxis;
	double lastPropertyAngle;
	double rotationDegX;
	double rotationDegY;
	double rotationDegZ;
	osg::Matrix totalRotation;
	osg::Matrix workingPlaneRotation;
};

