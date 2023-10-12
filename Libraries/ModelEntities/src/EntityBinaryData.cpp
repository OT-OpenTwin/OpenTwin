
#include "EntityBinaryData.h"
#include "DataBase.h"
#include "Types.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityBinaryData::EntityBinaryData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms,  factory, owner)
{
	
}

EntityBinaryData::~EntityBinaryData()
{
	data.clear();
}

bool EntityBinaryData::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}


void EntityBinaryData::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Now add the actual data array

	bsoncxx::types::b_binary bin_data;
	bin_data.size = data.size();
	bin_data.bytes = (uint8_t *)(data.data());

	storage.append(
		bsoncxx::builder::basic::kvp("size", (long long) data.size()),
		bsoncxx::builder::basic::kvp("data", bin_data)
	);
}

void EntityBinaryData::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now read the curve data

	data.clear();

	// Now load the data from the storage

	auto bin_data = doc_view["data"].get_binary();

	data.reserve(bin_data.size);

	for (unsigned long index = 0; index < bin_data.size; index++)
	{
		data.push_back(bin_data.bytes[index]);
	}

	resetModified();
}

void EntityBinaryData::removeChild(EntityBase *child)
{
	EntityBase::removeChild(child);
}

void EntityBinaryData::setData(const char *dat, size_t length)
{
	data.clear();
	data.reserve(length);

	for (unsigned long index = 0; index < length; index++)
	{
		data.push_back(dat[index]);
	}
}

void EntityBinaryData::clearData(void)
{
	data.clear();
}

const std::vector<char> &EntityBinaryData::getData(void)
{
	return data;
}

