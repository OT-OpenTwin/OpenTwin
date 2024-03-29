#pragma once
#pragma once

#include "EntityResultTable.h"

#include <string>
#include <stdint.h>
#include <vector>
#include <map>

class __declspec(dllexport) EntityParameterizedDataPreviewTable : public EntityResultTable<std::string>
{
public:

	EntityParameterizedDataPreviewTable(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual std::string getClassName(void) { return "EntityParameterizedDataPreviewTable"; };
	
	std::vector<std::pair<ot::UID, ot::UID>> GetLastStatusOfPreview() { return _displayedRanges; };
	void AddRangeToPreviewStatus(std::pair<ot::UID, ot::UID> range) { _displayedRanges.push_back(range); }

private:
	std::vector<std::pair<ot::UID, ot::UID>> _displayedRanges;


	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;


};
