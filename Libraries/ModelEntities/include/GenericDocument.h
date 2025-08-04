#pragma once
#include <map>
#include <stdint.h>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include "OTCore/Variable.h"

class __declspec(dllexport) GenericDocument
{
public:
	const std::string getDocumentName() const { return m_documentName; }
	const std::vector<const GenericDocument *> getSubDocuments() const { return m_subDocuments; }
	void setDocumentName(std::string _documentName) { m_documentName = _documentName; };
	const std::map<std::string, std::list<ot::Variable>>* getFields() const { return &m_fields; };

	void addSubDocument(GenericDocument* _subDocument) { m_subDocuments.push_back(_subDocument); };
	void clearSubDocuments() { m_subDocuments.clear(); }
	void insertInDocumentField(const std::string& _fieldName, const std::list<ot::Variable>& _values);
	void insertInDocumentField(const std::string& _fieldName, std::list<ot::Variable>&& _values);

protected:

	std::map<std::string, std::list<ot::Variable>> m_fields;
	std::string m_documentName;
	std::vector<const GenericDocument*> m_subDocuments;

	virtual void checkForIlligalName(std::string _fieldName) {};
};