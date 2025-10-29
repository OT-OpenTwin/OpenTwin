// @otlicense

#pragma once
#include "Document/DocumentAccess.h"
#pragma warning(disable:4275)
/*
* Warning C4275: non - DLL-interface class 'class_1' used as base for DLL-interface class 'class_2'
* Is ok, as long as no static data or CRT functionality are used under certain conditions (https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN)
* Neither is currently used in the base class so it should be alright.
*/


class __declspec(dllexport) UnittestDocumentAccess : public DataStorageAPI::DocumentAccess
{
public:
	UnittestDocumentAccess(const std::string& collectionName);
};
