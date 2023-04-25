#pragma once
#include <map>
#include <stdint.h>
#include <list>
#include <vector>
#include <string>
#include <memory>

class __declspec(dllexport) GenericDocument
{
	using StringFieldsByName = std::map<std::string, std::list<std::string>>;
	using DoubleFieldsByName = std::map<std::string, std::list<double>>;
	using Int32FieldsByName = std::map<std::string, std::list<int32_t>>;
	using Int64FieldsByName = std::map<std::string, std::list<int64_t>>;

public:

	const StringFieldsByName* getStringFields() const { return &_stringFieldsByName; };
	const DoubleFieldsByName* getDoubleFields() const { return &_doubleFieldsByName; };
	const Int32FieldsByName* getInt32Fields() const { return &_int32FieldsByName; };
	const Int64FieldsByName* getInt64Fields() const { return &_int64FieldsByName; };

	const std::string getDocumentName() const { return _documentName; }
	void setDocumentName(std::string documentName) { _documentName = documentName; };

	const std::vector<const GenericDocument *> getSubDocuments() const { return _subDocuments; }
	void AddSubDocument(GenericDocument* subDocument) { _subDocuments.push_back(subDocument); };

	template<class T>
	void InsertInDocumentField(std::string fieldName, std::list<T> values);

protected:
	std::string _documentName;
	std::vector<const GenericDocument*> _subDocuments;

	StringFieldsByName _stringFieldsByName;
	DoubleFieldsByName _doubleFieldsByName;
	Int32FieldsByName _int32FieldsByName;
	Int64FieldsByName _int64FieldsByName;

	virtual void CheckForIlligalName(std::string fieldName) {};
};



template<class T>
inline void GenericDocument::InsertInDocumentField(std::string fieldName, std::list<T> values)
{
	//static_assert(false, "Function template only supports int32, int64, double and string.");
}

template<>
inline void GenericDocument::InsertInDocumentField(std::string fieldName, std::list<std::string> values)
{
	//CheckForIlligalName(fieldName);
	_stringFieldsByName[fieldName].insert(_stringFieldsByName[fieldName].end(), values.begin(), values.end());
}

template<>
inline void GenericDocument::InsertInDocumentField(std::string fieldName, std::list<double> values)
{
	//CheckForIlligalName(fieldName);
	_doubleFieldsByName[fieldName].insert(_doubleFieldsByName[fieldName].end(), values.begin(), values.end());
}

template<>
inline void GenericDocument::InsertInDocumentField(std::string fieldName, std::list<int32_t> values)
{
	//CheckForIlligalName(fieldName);
	_int32FieldsByName[fieldName].insert(_int32FieldsByName[fieldName].end(), values.begin(), values.end());
}

template<>
inline void GenericDocument::InsertInDocumentField(std::string fieldName, std::list<int64_t> values)
{
	//CheckForIlligalName(fieldName);
	_int64FieldsByName[fieldName].insert(_int64FieldsByName[fieldName].begin(), values.begin(), values.end());
}
