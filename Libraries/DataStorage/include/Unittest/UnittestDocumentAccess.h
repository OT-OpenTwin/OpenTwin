// @otlicense
// File: UnittestDocumentAccess.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
