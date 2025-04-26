//! @file Exception.h
//! @author Alexander Kuester (alexk95)
//! @date April 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// std header
#include <string>
#include <exception>

#pragma warning (disable: 4251)

namespace ot {

    class Exception : public std::exception {
    public:
        explicit Exception(const std::string& _message) : std::exception(_message.c_str()) {};
        explicit Exception(const char* _message) : std::exception(_message) {};
    };

    // ###########################################################################################################################################################################################################################################################################################################################

    // Data

    //! @brief Basic object not found exception.
    class ObjectNotFoundException : public Exception {
    public:
        explicit ObjectNotFoundException() : Exception("Object not found") {};
        explicit ObjectNotFoundException(const std::string& _message) : Exception(_message.c_str()) {};
        explicit ObjectNotFoundException(const char* _message) : Exception(_message) {};
    };
    namespace Exception { typedef ot::ObjectNotFoundException ObjectNotFound; }

    // ###########################################################################################################################################################################################################################################################################################################################

    // Network

    //! @brief Basic network error exception.
    class NetworkErrorException : public Exception {
    public:
        explicit NetworkErrorException() : Exception("Network error") {};
        explicit NetworkErrorException(const std::string& _message) : Exception(_message.c_str()) {};
        explicit NetworkErrorException(const char* _message) : Exception(_message) {};
    };
    namespace Exception { typedef ot::NetworkErrorException NetworkError; }


}
