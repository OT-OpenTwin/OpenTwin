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
	const std::string getDocumentName() const { return _documentName; }
	const std::vector<const GenericDocument *> getSubDocuments() const { return _subDocuments; }
	void setDocumentName(std::string documentName) { _documentName = documentName; };
	const std::map<std::string, std::list<ot::Variable>>* getFields() const { return &_fields; };

	void AddSubDocument(GenericDocument* subDocument) { _subDocuments.push_back(subDocument); };

	void InsertInDocumentField(const std::string& fieldName, std::list<ot::Variable>& values);
	void InsertInDocumentField(const std::string& fieldName, std::list<ot::Variable>&& values);

protected:

	std::map<std::string, std::list<ot::Variable>> _fields;
	std::string _documentName;
	std::vector<const GenericDocument*> _subDocuments;

	virtual void CheckForIlligalName(std::string fieldName) {};
};