#include "EntityBlock.h"

EntityBlock::EntityBlock(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactory* factory, const std::string& owner)
	:EntityBase(ID,parent,obs,ms,factory,owner), _blockID(0), _location(0.,0.)
{
}

void EntityBlock::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::AddStorageData(storage);
	storage.append(
		bsoncxx::builder::basic::kvp("BlockID", static_cast<int64_t>(_blockID)),
		bsoncxx::builder::basic::kvp("LocationX", _location.x()),
		bsoncxx::builder::basic::kvp("LocationY", _location.y())
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
	double locationX = doc_view["LocationX"].get_double().value;
	double locationY = doc_view["LocationY"].get_double().value;
	_location.setX(locationX);
	_location.setY(locationY);

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
