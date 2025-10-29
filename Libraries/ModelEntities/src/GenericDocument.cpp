// @otlicense

#include "GenericDocument.h"

void GenericDocument::insertInDocumentField(const std::string& _fieldName, const std::list<ot::Variable>& _values)
{
	//CheckForIlligalName(fieldName);
	m_fields[_fieldName].insert(m_fields[_fieldName].begin(), _values.begin(), _values.end());
}

void GenericDocument::insertInDocumentField(const std::string& _fieldName, std::list<ot::Variable>&& _values)
{
	//CheckForIlligalName(fieldName);
	m_fields[_fieldName].insert(m_fields[_fieldName].begin(), _values.begin(), _values.end());
}