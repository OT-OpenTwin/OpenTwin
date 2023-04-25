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
	void AddAllDocumentFieldsToDocument(bsoncxx::builder::basic::document& storage) const;

private:
	std::vector<char> _forbiddenFieldNameContent{ '$', '.' };
	template<class T>
	void AddMapToDocument(std::map < std::string, std::list<T>> map, bsoncxx::builder::basic::document& storage) const;
	//virtual void CheckForIlligalName(std::string fieldName) override;
};

template<class T>
inline void GenericBsonDocument::AddMapToDocument(std::map < std::string, std::list<T>> map, bsoncxx::builder::basic::document& storage) const
{
	for (auto mapEntry : map)
	{
		std::list<T> fieldValues = mapEntry.second;
		if (fieldValues.size() == 1)
		{
			storage.append(bsoncxx::builder::basic::kvp(mapEntry.first, *fieldValues.begin()));
		}
		else
		{
			auto valueArray = bsoncxx::builder::basic::array();
			for (auto fieldValue : fieldValues)
			{
				valueArray.append(fieldValue);
			}
			storage.append(bsoncxx::builder::basic::kvp(mapEntry.first, valueArray));
		}
	}
}