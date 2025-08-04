//! @file JSONTypes.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSONTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/CoreAPIExport.h"

// rapidjson header
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

// std header
#include <list>

#pragma warning(disable:4251)

namespace ot {

	//! @brief Size type
	typedef rapidjson::SizeType JsonSizeType;

	//! @brief Writable JSON value
	typedef rapidjson::Value JsonValue;

	//! @brief Allocator used for writing to JSON values
	typedef rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> JsonAllocator;

	//typedef rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> JsonObjectHandle;

	//! @brief Read only JSON Object
	typedef rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> ConstJsonObject;

	typedef std::list<ConstJsonObject> ConstJsonObjectList;

	//typedef rapidjson::GenericArray<false, rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> JsonArrayHandle;

	//! @brief Read only JSON Array
	typedef rapidjson::GenericArray<true, rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> ConstJsonArray;

	typedef std::list<ConstJsonArray> ConstJsonArrayList;

	//! @brief Read only JSON Object iterator
	typedef rapidjson::GenericMemberIterator<true, rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> JsonMemberIterator;



}