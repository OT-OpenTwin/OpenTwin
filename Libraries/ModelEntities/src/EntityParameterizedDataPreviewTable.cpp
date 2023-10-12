#include "EntityParameterizedDataPreviewTable.h"

EntityParameterizedDataPreviewTable::EntityParameterizedDataPreviewTable(ot::UID ID, EntityBase * parent, EntityObserver * mdl, ModelState * ms, ClassFactoryHandler* factory, const std::string & owner)
	: EntityResultTable<std::string>(ID, parent, mdl, ms, factory, owner)
{
}

void EntityParameterizedDataPreviewTable::AddStorageData(bsoncxx::builder::basic::document & storage)
{
	// We store the parent class information first 
	EntityResultTable::AddStorageData(storage);

	auto selectionRangesIDs = bsoncxx::builder::basic::array();
	auto selectionRangesVersions = bsoncxx::builder::basic::array();

	for (size_t i = 0; i < _displayedRanges.size(); i ++)
	{
		selectionRangesIDs.append(static_cast<int64_t>(_displayedRanges[i].first));
		selectionRangesVersions.append(static_cast<int64_t>(_displayedRanges[i].second));
	}

	// Now add the actual text data

	storage.append(
		bsoncxx::builder::basic::kvp("selectionRangesIDs", selectionRangesIDs),
		bsoncxx::builder::basic::kvp("selectionRangesVersions", selectionRangesVersions)
	);
}

void EntityParameterizedDataPreviewTable::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	// We read the parent class information first 
	EntityResultTable::readSpecificDataFromDataBase(doc_view, entityMap);

	auto selectionRangesIDs = doc_view["selectionRangesIDs"].get_array().value;
	auto selectionRangesVersions = doc_view["selectionRangesVersions"].get_array().value;

	std::vector<ot::UID> selectionID, selectionVersion;
	for (auto selectionRangeID : selectionRangesIDs)
	{
		selectionID.push_back(static_cast<ot::UID>(selectionRangeID.get_int64()));
	}
	for (auto selectionRangeVersion : selectionRangesVersions)
	{
		selectionVersion.push_back(static_cast<ot::UID>(selectionRangeVersion.get_int64()));
	}

	for (int i = 0; i < selectionVersion.size(); i++)
	{
		auto identifyer = std::make_pair<>(selectionID[i], selectionVersion[i]);
		_displayedRanges.push_back(identifyer);
	}

	resetModified();
}
