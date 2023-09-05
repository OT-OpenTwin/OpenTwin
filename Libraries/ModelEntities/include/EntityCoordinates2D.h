#pragma once
#include "EntityBase.h"
#include "OpenTwinCore/Point2D.h"

class __declspec(dllexport) EntityCoordinates2D : public EntityBase
{
public:
	EntityCoordinates2D(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactory* factory, const std::string& owner);
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override;
	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	void setCoordinates(ot::Point2DD& position) { _location = position; }
	ot::Point2DD getCoordinates() { return _location; }
	virtual std::string getClassName(void) { return "EntityCoordinates2D"; };
private:
	ot::Point2DD _location;

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};
