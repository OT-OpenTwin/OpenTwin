#include "EntityCoordinates2D.h"

EntityCoordinates2D::EntityCoordinates2D(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactory* factory, const std::string& owner)
	:EntityBase(ID,parent,obs,ms,factory,owner), _location(0.,0.)
{

}

bool EntityCoordinates2D::getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
	return false;
}

void EntityCoordinates2D::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::AddStorageData(storage);
	storage.append(
		bsoncxx::builder::basic::kvp("LocationX", _location.x()),
		bsoncxx::builder::basic::kvp("LocationY", _location.y())
	);
}

void EntityCoordinates2D::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	double locationX = doc_view["LocationX"].get_double().value;
	double locationY = doc_view["LocationY"].get_double().value;
	_location.setX(locationX);
	_location.setY(locationY);
}
