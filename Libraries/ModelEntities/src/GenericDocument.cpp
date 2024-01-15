#include "GenericDocument.h"

void GenericDocument::InsertInDocumentField(const std::string& fieldName, const std::list<ot::Variable>& values)
{
	//CheckForIlligalName(fieldName);
	_fields[fieldName].insert(_fields[fieldName].begin(), values.begin(), values.end());
}

void GenericDocument::InsertInDocumentField(const std::string& fieldName, std::list<ot::Variable>&& values)
{
	//CheckForIlligalName(fieldName);
	_fields[fieldName].insert(_fields[fieldName].begin(), values.begin(), values.end());
}