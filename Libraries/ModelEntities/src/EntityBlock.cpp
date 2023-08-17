#include "EntityBlock.h"

EntityBlock::EntityBlock(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactory* factory, const std::string& owner)
	:EntityBase(ID,parent,obs,ms,factory,owner)
{
}

void EntityBlock::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::AddStorageData(storage);
	storage.append(
		bsoncxx::builder::basic::kvp("BlockName", _blockName),
		bsoncxx::builder::basic::kvp("LocationX", _location.x()),
		bsoncxx::builder::basic::kvp("LocationY", _location.y())
	);
}

void EntityBlock::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	_blockName = doc_view["BlockName"].get_utf8().value.to_string();
	double locationX = doc_view["LocationX"].get_double().value;
	double locationY = doc_view["LocationY"].get_double().value;
	_location.setX(locationX);
	_location.setY(locationY);

}
