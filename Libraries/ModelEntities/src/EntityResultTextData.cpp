
#include "EntityResultTextData.h"
#include "DataBase.h"
#include "Types.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityResultTextData::EntityResultTextData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms,  factory, owner)
{
	
}

EntityResultTextData::~EntityResultTextData()
{
}

bool EntityResultTextData::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityResultTextData::StoreToDataBase(void)
{
	EntityBase::StoreToDataBase();
}

void EntityResultTextData::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Now add the actual text data

	storage.append(
		bsoncxx::builder::basic::kvp("textData", textData)
	);
}

void EntityResultTextData::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now read the text data
	textData.clear();

	// Now load the data from the storage
	textData = doc_view["textData"].get_utf8().value.data();

	resetModified();
}

void EntityResultTextData::removeChild(EntityBase *child)
{
	EntityBase::removeChild(child);
}

void EntityResultTextData::setText(const std::string &text)
{
	textData = text;
	setModified();
}

std::string &EntityResultTextData::getText(void)
{
	return textData;
}
