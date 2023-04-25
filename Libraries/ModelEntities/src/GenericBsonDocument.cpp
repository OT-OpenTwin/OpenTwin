#include "GenericBsonDocument.h"

void GenericBsonDocument::AddAllDocumentFieldsToDocument(bsoncxx::builder::basic::document& storage) const
{
	AddMapToDocument<>(_stringFieldsByName, storage);
	AddMapToDocument<>(_doubleFieldsByName, storage);
	AddMapToDocument<>(_int32FieldsByName, storage);
	AddMapToDocument<>(_int64FieldsByName, storage);
}

//void GenericBsonDocument::CheckForIlligalName(std::string fieldName)
//{
//	for (char& forbiddenCharacters : _forbiddenFieldNameContent)
//	{
//		if (fieldName.find(forbiddenCharacters) != std::string::npos)
//		{
//			std::string errorMessage = "Character  is not permitted to be contained in a fieldname.\n";
//			throw std::exception(errorMessage.insert(10, 1, forbiddenCharacters).c_str());
//		}
//	}
//}
