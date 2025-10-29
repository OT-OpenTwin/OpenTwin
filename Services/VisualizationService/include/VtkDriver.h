// @otlicense

#pragma once

#include <string>
#include "OldTreeIcon.h"
#include <ctime>

class EntityVis2D3D;
class DataSourceManagerItem;

class VtkDriver
{
public:
	VtkDriver();
	virtual ~VtkDriver();

	virtual void setProperties(EntityVis2D3D *visEntity) = 0;
	virtual std::string buildSceneNode(DataSourceManagerItem *dataItem) = 0;

	std::list<ot::UID> GetUpdateTopoEntityID() { return updateTopoEntityID; }
	std::list<ot::UID> GetUpdateTopoEntityVersion() { return updateTopoEntityVersion; }
	std::list<bool> GetUpdateTopoEntityForceVisible() { return updateTopoForceVisibility; }

protected:
	void reportTime(const std::string &message, std::time_t &timer);

	std::list<ot::UID> updateTopoEntityID;
	std::list<ot::UID> updateTopoEntityVersion;
	std::list<bool> updateTopoForceVisibility;
};
