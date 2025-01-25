#include "EntityCoordinates2D.h"

EntityCoordinates2D::EntityCoordinates2D(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBase(ID,parent,obs,ms,factory,owner), m_location(0.,0.)
{

}

bool EntityCoordinates2D::getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
	return false;
}

EntityBase* EntityCoordinates2D::clone()
{
	auto clonedEntity = std::make_unique<EntityCoordinates2D>(*this);
	clonedEntity->setObserver(nullptr);
	clonedEntity->setParent(nullptr);
	return clonedEntity.release();
}

void EntityCoordinates2D::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::AddStorageData(storage);
	storage.append(
		bsoncxx::builder::basic::kvp("LocationX", m_location.x()),
		bsoncxx::builder::basic::kvp("LocationY", m_location.y())
	);
}

void EntityCoordinates2D::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	double locationX = doc_view["LocationX"].get_double().value;
	double locationY = doc_view["LocationY"].get_double().value;
	m_location.setX(locationX);
	m_location.setY(locationY);
}
