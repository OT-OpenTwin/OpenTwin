// @otlicense
// File: ContextMenuRequestEvent.cpp
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

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"
#include "OTGui/Event/ContextMenuRequestEvent.h"

ot::ContextMenuRequestEvent::ContextMenuRequestEvent(ContextRequestData* _data)
	: m_data(_data)
{

}

ot::ContextMenuRequestEvent::ContextMenuRequestEvent(std::unique_ptr<ContextRequestData>&& _data)
	: m_data(std::move(_data))
{

}

ot::ContextMenuRequestEvent::ContextMenuRequestEvent(const ConstJsonObject& _jsonObject) : ContextMenuRequestEvent()
{
	setFromJsonObject(_jsonObject);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::ContextMenuRequestEvent::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	GuiEvent::addToJsonObject(_object, _allocator);
	
	if (m_data)
	{
		_object.AddMember("Data", JsonObject(m_data.get(), _allocator), _allocator);
	}
	else
	{
		_object.AddMember("Data", JsonNullValue(), _allocator);
	}
}

void ot::ContextMenuRequestEvent::setFromJsonObject(const ConstJsonObject& _object)
{
	GuiEvent::setFromJsonObject(_object);

	auto dat = _object.FindMember("Data");
	if (dat == _object.MemberEnd() || !dat->value.IsObject())
	{
		setRequestData(nullptr);
	}
	else
	{
		ContextRequestData* data = ContextRequestData::fromJson(dat->value.GetObject());
		OTAssertNullptr(data);
		setRequestData(data);
	}
}
