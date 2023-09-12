#pragma once

#include <vector>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include "GenericDocument.h"

class GenericBsonDocument : public GenericDocument
{
public:
	void AddAllFieldsToDocument(bsoncxx::builder::basic::document& storage) const;

private:
	std::vector<char> _forbiddenFieldNameContent{ '$', '.' };
	
	//virtual void CheckForIlligalName(std::string fieldName) override;
};