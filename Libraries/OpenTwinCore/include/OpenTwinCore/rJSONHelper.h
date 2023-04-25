#pragma once

#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/Logger.h"

//! @brief Checks if the provided json object has the specified member
//! @param ___object The JSON object
//! @param ___memberName The name of the object member
#define OT_rJSON_checkMemberExists(___object, ___memberName) if (!___object.HasMember(___memberName)) { OT_LOG_E("JSON object member \"" + std::string(___memberName) + "\" is missing"); throw std::exception("JSON object member missing"); }

//! @brief Checks if the provided json object member has the correct type
//! @param ___object The JSON object
//! @param ___memberName The name of the object member
//! @param ___memberType The expected type of the object member
#define OT_rJSON_checkMemberType(___object, ___memberName, ___memberType) if (!___object[___memberName].Is##___memberType()) { OT_LOG_E("JSON object member \"" + std::string(___memberName) + "\" is not a " #___memberType); throw std::exception("JSON object member invalid type"); }

//! @brief Checks if the specified json object member exists and has the correct type
//! @param ___object The JSON object
//! @param ___memberName The name of the object member
//! @param ___memberType The expected type of the object member
#define OT_rJSON_checkMember(___object, ___memberName, ___memberType) OT_rJSON_checkMemberExists(___object, ___memberName); OT_rJSON_checkMemberType(___object, ___memberName, ___memberType)

//! @brief Checks if the json array entry at the given index has the correct type
//! @param ___array The JSON array
//! @param ___index The index
//! @param ___entryType The expected type of the entry
#define OT_rJSON_checkArrayEntryType(___array, ___index, ___entryType) if (!___array[___index].Is##___entryType()) { OT_LOG_E("JSON array entry at index \"" + std::to_string(___index) + "\" is not a " #___entryType); throw std::exception("JSON array entry invalid type"); }