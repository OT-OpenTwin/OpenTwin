// @otlicense
// File: MetadataEntryComperator.cpp
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

#include "MetadataEntryComperator.h"
#include "MetadataEntryArray.h"
#include "MetadataEntrySingle.h"
#include "MetadataEntryObject.h"

bool MetadataEntryComperator::operator()(std::shared_ptr<MetadataEntry> one, std::shared_ptr<MetadataEntry> two)
{
	auto otherMetadataEntryArray = dynamic_cast<MetadataEntryArray*>(one.get());
	if (otherMetadataEntryArray != nullptr)
	{
		auto metadataEntryArray = dynamic_cast<MetadataEntryArray*>(two.get());
		if (metadataEntryArray == nullptr)
		{
			return false;
		}
		else
		{
			return (*otherMetadataEntryArray) == (*metadataEntryArray);
		}
	}

	auto otherMetadataEntrySingle = dynamic_cast<MetadataEntrySingle*>(one.get());
	if (otherMetadataEntrySingle != nullptr)
	{
		auto metadataEntrySingle = dynamic_cast<MetadataEntrySingle*>(two.get());
		if (metadataEntrySingle == nullptr)
		{
			return false;
		}
		else
		{
			return (*otherMetadataEntrySingle) == (*metadataEntrySingle);
		}
	}

	auto otherMetadataEntryObject = dynamic_cast<MetadataEntryObject*>(one.get());
	if (otherMetadataEntryObject != nullptr)
	{
		auto metadataEntryObject = dynamic_cast<MetadataEntryObject*>(two.get());
		if (metadataEntryObject == nullptr)
		{
			return false;
		}
		else
		{
			return (*otherMetadataEntryObject) == (*metadataEntryObject);
		}
	}

	return false;
}
