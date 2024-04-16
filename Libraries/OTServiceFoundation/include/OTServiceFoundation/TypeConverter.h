#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTServiceFoundation/FoundationAPIExport.h"
#include "OTServiceFoundation/FoundationTypes.h"

// std header
#include <list>
#include <string>
#include <sstream>

namespace ot {

	namespace convert {

		//! @brief Will return a string representation of the provided flag
		//! @param _flag The flag, must be only one flag
		OT_SERVICEFOUNDATION_API_EXPORT std::string toString(ot::ItemCheckedState _flag);

		//! @brief Will return a string representation of the provided flag
		//! @param _flag The flag, must be only one flag
		OT_SERVICEFOUNDATION_API_EXPORT std::string toString(ot::ItemFlag _flag);

		//! @brief Will return a list with all flags that are set as a string
		//! @param _flags The flags
		OT_SERVICEFOUNDATION_API_EXPORT std::list<std::string> toString(ItemCheckedStateFlags _flags);

		//! @brief Will return a list with all flags that are set as a string
		//! @param _flags The flags
		OT_SERVICEFOUNDATION_API_EXPORT std::list<std::string> toString(ItemFlags _flags);

		//! @brief Will return a flags object containing all set flags that are provided in the list
		//! @param _flags The list containing all flags that should be set
		OT_SERVICEFOUNDATION_API_EXPORT ot::ItemCheckedState toItemCheckedState(const std::string& _flags);

		//! @brief Will return a flags object containing all set flags that are provided in the list
		//! @param _flags The list containing all flags that should be set
		OT_SERVICEFOUNDATION_API_EXPORT ot::ItemFlag toItemFlag(const std::string& _flags);

		//! @brief Will return a flags object containing all set flags that are provided in the list
		//! @param _flags The list containing all flags that should be set
		OT_SERVICEFOUNDATION_API_EXPORT ItemCheckedStateFlags toItemCheckedStateFlags(const std::list<std::string>& _flags);

		//! @brief Will return a flags object containing all set flags that are provided in the list
		//! @param _flags The list containing all flags that should be set
		OT_SERVICEFOUNDATION_API_EXPORT ItemFlags toItemFlags(const std::list<std::string>& _flags);

		//! @brief Will convert and return the number specified in the string
		//! @param _string The string containing the number
		//! @param _failed Reference to a flag where the failed state will be written to
		template <class T> T toNumber(const std::string& _string, bool& _failed);

	}

}

template <class T> T ot::convert::toNumber(const std::string& _string, bool& _failed) {
	std::stringstream ss(_string);
	T v;
	ss >> v;
	_failed = false;
	if (ss.fail()) { _failed = true; }
	std::string rest;
	ss >> rest;
	if (!rest.empty()) { _failed = true; }
	return v;
}
