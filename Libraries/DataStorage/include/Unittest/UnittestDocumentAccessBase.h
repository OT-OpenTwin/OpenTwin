#pragma once
#include <string>
#include "Document/DocumentAccessBase.h"

#pragma warning(disable : 4251)
#pragma warning(disable:4275)
/*
* Warning C4275: non - DLL-interface class 'class_1' used as base for DLL-interface class 'class_2'
* Is ok, as long as no static data or CRT functionality are used under certain conditions (https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN)
* Neither is currently used in the base class so it should be alright.
*/
class __declspec(dllexport) UnittestDocumentAccessBase : public DataStorageAPI::DocumentAccessBase
{
public:
	UnittestDocumentAccessBase(const std::string& collectionName);

private:
	const std::string _serverURL = "mongodb://localhost:27013";
	const std::string _dbName = "Unittests";
};
