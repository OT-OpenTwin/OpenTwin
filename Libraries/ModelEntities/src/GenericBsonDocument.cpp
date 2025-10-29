// @otlicense

#include "GenericBsonDocument.h"
#include "VariableToBSONConverter.h"
void GenericBsonDocument::addAllFieldsToDocument(bsoncxx::builder::basic::document& _storage) const
{
	VariableToBSONConverter converter;
	for (auto mapEntry : m_fields)
	{
		std::list<ot::Variable>& fieldValues = mapEntry.second;
		const std::string fieldName = mapEntry.first;
		if (fieldValues.size() == 1)
		{
			const auto& value = *fieldValues.begin();
			converter(_storage,value,fieldName);
		}
		else
		{
			auto valueArray = bsoncxx::builder::basic::array();
			for (auto& fieldValue : fieldValues)
			{
				converter(valueArray, fieldValue);
			}
			_storage.append(bsoncxx::builder::basic::kvp(fieldName, valueArray));
		}
	}
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
