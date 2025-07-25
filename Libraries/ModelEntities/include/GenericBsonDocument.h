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
	void addAllFieldsToDocument(bsoncxx::builder::basic::document& _storage) const;

private:
	std::vector<char> m_forbiddenFieldNameContent{ '$', '.' };
	
	//virtual void CheckForIlligalName(std::string fieldName) override;
};