// @otlicense
// File: GenericDataStructFactory.cpp
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

#include "OTCore/GenericDataStructFactory.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructSingle.h"
ot::GenericDataStruct* ot::GenericDataStructFactory::Create(const ot::ConstJsonObject& object)
{
	ot::GenericDataStruct temp;
	temp.setFromJsonObject(object);
	std::string type = temp.getTypeIdentifyer();
	if (type == ot::GenericDataStructMatrix::getClassName())
	{
		ot::GenericDataStruct* returnVal(new ot::GenericDataStructMatrix());
		returnVal->setFromJsonObject(object);
		return returnVal;
	}
	else if(type == ot::GenericDataStructVector::getClassName())
	{
		ot::GenericDataStruct* returnVal(new ot::GenericDataStructVector());
		returnVal->setFromJsonObject(object);
		return returnVal;
	}
	else if (type == ot::GenericDataStructSingle::getClassName())
	{
		ot::GenericDataStruct* returnVal(new ot::GenericDataStructSingle());
		returnVal->setFromJsonObject(object);
		return returnVal;
	}
	else
	{
		throw std::exception("Not supported GenericDataStruct type");
	}
}
