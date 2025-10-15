#include "EntityCoordinates2D.h"

static EntityFactoryRegistrar<EntityCoordinates2D> registrar(EntityCoordinates2D::className());

EntityCoordinates2D::EntityCoordinates2D(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBase(ID, parent, obs, ms, owner), m_location(0.,0.)
{

}

bool EntityCoordinates2D::getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
	return false;
}

std::string EntityCoordinates2D::serialiseAsJSON()
{
	auto doc = EntityBase::serialiseAsMongoDocument();
	const std::string jsonDoc = bsoncxx::to_json(doc);
	return jsonDoc;
}

bool EntityCoordinates2D::deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept
{
	try
	{
		const std::string serialisationString = ot::json::toJson(_serialisation);
		std::string_view serialisedEntityJSONView(serialisationString);
		auto serialisedEntityBSON = bsoncxx::from_json(serialisedEntityJSONView);
		auto serialisedEntityBSONView = serialisedEntityBSON.view();
	
		readSpecificDataFromDataBase(serialisedEntityBSONView, _entityMap);
		setEntityID(createEntityUID());
		
		ot::Point2DD newPosition = _copyInformation.getDestinationScenePos();
		setCoordinates(newPosition);
		return true;
	}
	catch (std::exception& _e)
	{
		OT_LOG_E("Failed to deserialise EntityCoordinates2D because: " + std::string(_e.what()));
		return false;
	}
}

void EntityCoordinates2D::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::addStorageData(storage);
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
