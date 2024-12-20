#pragma once

#include "OldTreeIcon.h"

#include <string>
#include <vector>
#include <list>

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

class EntityGeometry;
class EntityCache;
class ClassFactory;

class TopoDS_Shape;

class ShapesBase
{
public:
	ShapesBase(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, ot::serviceID_t _serviceID, const std::string &_serviceName, EntityCache *_entityCache, ClassFactory *_classFactory);
	~ShapesBase() {};

	ShapesBase() = delete;

protected:
	std::string to_string(double value);
	void storeShapeInModel(const TopoDS_Shape & _shape, std::vector<double> &_transform, const std::string& _name, 
						   const std::string &_type, std::list<std::pair<std::string, std::string>> &shapeParameters, std::list<std::string> &faceNames);
	void applyFaceNames(EntityGeometry *geomEntity, const TopoDS_Shape& _shape, std::list<std::string>& faceNames);

	void writeShapeToStepFile(const TopoDS_Shape & _shape, const std::string& _filename);
	void deleteNonStandardProperties(EntityGeometry* geometryEntity);

protected:
	ot::components::UiComponent *uiComponent;
	ot::components::ModelComponent *modelComponent;
	ot::serviceID_t serviceID;
	std::string serviceName;
	EntityCache *entityCache;
	ClassFactory* classFactory;

private:
	static std::string materialsFolder;
	static ot::UID materialsFolderID;
};
