#include "EntityBlock.h"

EntityBlock::EntityBlock(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactory* factory, const std::string& owner)
	:EntityBase(ID,parent,obs,ms,factory,owner)
{
}

ot::Point2DD EntityBlock::getCoordinates() 
{
	if (_coordinate2DEntityID != 0)
	{
		if (_coordinate2DEntity == nullptr)
		{
			ot::UID currentVersion = getCurrentEntityVersion(_coordinate2DEntityID);
			if (currentVersion != _coordinate2DEntity->getEntityStorageVersion())
			{
				std::map<ot::UID, EntityBase*> entitymap;
				auto entBase = readEntityFromEntityID(this, _coordinate2DEntityID, entitymap);
				_coordinate2DEntity = dynamic_cast<EntityCoordinates2D*>(entBase);
				assert(_coordinate2DEntity != nullptr);
			}
		}
		return _coordinate2DEntity->getCoordinates();
	}
	else
	{
		throw std::exception("Coordinates of Entityblock were not set.");
	}
}

void EntityBlock::setCoordinates(ot::Point2DD& coordinates)
{
	if (_coordinate2DEntity != nullptr)
	{
		delete _coordinate2DEntity;
		_coordinate2DEntity = nullptr;
		_coordinate2DEntityID = 0;
	}
	
	_coordinate2DEntity = new EntityCoordinates2D(createEntityUID(), this, getObserver(), getModelState(), getClassFactory(), getOwningService());
	_coordinate2DEntity->setCoordinates(coordinates);
	_coordinate2DEntity->StoreToDataBase();

	_coordinate2DEntityID = _coordinate2DEntity->getEntityID();
}

ot::UID EntityBlock::getCoordinateEntityID() const
{
	if (_coordinate2DEntity != nullptr)
	{
		return _coordinate2DEntity->getEntityID();
	}
	else
	{
		return 0;
	}
}

ot::UID EntityBlock::getCoordinateEntityVersion() const
{
	if (_coordinate2DEntity != nullptr)
	{
		return _coordinate2DEntity->getEntityStorageVersion();
	}
	else
	{
		return 0;
	}
}

void EntityBlock::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::AddStorageData(storage);
	storage.append(
		bsoncxx::builder::basic::kvp("BlockID", static_cast<int64_t>(_blockID)),
		bsoncxx::builder::basic::kvp("CoordinatesEntityID", static_cast<int64_t>(_coordinate2DEntityID))
	);
	
	auto connectorsArray = bsoncxx::builder::basic::array();
	for (const ot::Connector& connector : _connectors)
	{
		auto subDocument = connector.SerializeBSON();
		connectorsArray.append(subDocument);
	}
	storage.append(bsoncxx::builder::basic::kvp("Connectors", connectorsArray));

	auto outgoingConnectionArray = bsoncxx::builder::basic::array();
	for (ot::BlockConnection& connection : _outgoingConnections)
	{
		auto subDocument = connection.SerializeBSON();
		outgoingConnectionArray.append(subDocument);
	}
	storage.append(bsoncxx::builder::basic::kvp("Connections", outgoingConnectionArray));
}

void EntityBlock::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	_blockID = static_cast<ot::UID>(doc_view["BlockID"].get_int64());
	_coordinate2DEntityID = static_cast<ot::UID>(doc_view["CoordinatesEntityID"].get_int64());
	
	auto allOutgoingConnections	= doc_view["Connections"].get_array();
	for (auto& element : allOutgoingConnections.value)
	{
		auto subDocument = element.get_value().get_document();
		ot::BlockConnection connection;
		connection.DeserializeBSON(subDocument);
		_outgoingConnections.push_back(connection);
	}

	auto allConnectors = doc_view["Connectors"].get_array();
	for (auto& element: allConnectors.value)
	{
		auto subDocument = element.get_value().get_document();
		ot::Connector connector;
		connector.DeserializeBSON(subDocument);
		_connectors.push_back(connector);
	}
}
