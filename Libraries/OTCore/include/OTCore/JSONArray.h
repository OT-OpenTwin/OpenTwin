//! @file JsonArray.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSONTypes.h"
#include "OTCore/CoreTypes.h"

// std header
#include <list>
#include <string>
#include <vector>

namespace ot {

	//! @class JsonArray
	//! @brief JSON Array value
	class JsonArray : public JsonValue {
		OT_DECL_NOCOPY(JsonArray)
		OT_DECL_DEFMOVE(JsonArray)
	public:
		JsonArray();

		//! @brief Create bool array
		explicit JsonArray(const std::list<bool>& _lst, JsonAllocator& _allocator);

		//! @brief Create bool array
		explicit JsonArray(const std::vector<bool>& _vec, JsonAllocator& _allocator);

		//! @brief Create int32 array from int16 list
		explicit JsonArray(const std::list<int16_t>& _lst, JsonAllocator& _allocator);

		//! @brief Create int32 array from int16 vector
		explicit JsonArray(const std::vector<int16_t>& _vec, JsonAllocator& _allocator);

		//! @brief Create uint32 array from uint16 list
		explicit JsonArray(const std::list<uint16_t>& _lst, JsonAllocator& _allocator);

		//! @brief Create uint32 array from uint16 vector
		explicit JsonArray(const std::vector<uint16_t>& _vec, JsonAllocator& _allocator);

		//! @brief Create int32 array
		explicit JsonArray(const std::list<int32_t>& _lst, JsonAllocator& _allocator);

		//! @brief Create int32 array
		explicit JsonArray(const std::vector<int32_t>& _vec, JsonAllocator& _allocator);

		//! @brief Create uint32 array
		explicit JsonArray(const std::list<uint32_t>& _lst, JsonAllocator& _allocator);

		//! @brief Create uint32 array
		explicit JsonArray(const std::vector<uint32_t>& _vec, JsonAllocator& _allocator);

		//! @brief Create int64 array
		explicit JsonArray(const std::list<int64_t>& _lst, JsonAllocator& _allocator);

		//! @brief Create int64 array
		explicit JsonArray(const std::vector<int64_t>& _vec, JsonAllocator& _allocator);

		//! @brief Create uint64 array
		explicit JsonArray(const std::list<uint64_t>& _lst, JsonAllocator& _allocator);

		//! @brief Create uint64 array
		explicit JsonArray(const std::vector<uint64_t>& _vec, JsonAllocator& _allocator);

		//! @brief Create float array
		explicit JsonArray(const std::list<float>& _lst, JsonAllocator& _allocator);

		//! @brief Create float array
		explicit JsonArray(const std::vector<float>& _vec, JsonAllocator& _allocator);

		//! @brief Create double array
		explicit JsonArray(const std::list<double>& _lst, JsonAllocator& _allocator);

		//! @brief Create double array
		explicit JsonArray(const std::vector<double>& _vec, JsonAllocator& _allocator);

		//! @brief Create string array
		explicit JsonArray(const std::list<std::string>& _lst, JsonAllocator& _allocator);

		//! @brief Create string array
		explicit JsonArray(const std::vector<std::string>& _vec, JsonAllocator& _allocator);

		~JsonArray() {};
	};

}

#include "OTCore/JSONArray.hpp"