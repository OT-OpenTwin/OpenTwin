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

    class GeneralException : public std::exception {
    public:
        explicit GeneralException(const std::string& _message) : std::exception(_message.c_str()) {};
        explicit GeneralException(const char* _message) : std::exception(_message) {};
    };

    // ###########################################################################################################################################################################################################################################################################################################################

    // Data

    //! @brief Basic object not found exception.
    class ObjectNotFoundException : public GeneralException {
    public:
        explicit ObjectNotFoundException() : GeneralException("Object not found") {};
        explicit ObjectNotFoundException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit ObjectNotFoundException(const char* _message) : GeneralException(_message) {};
    };
    namespace Exception { typedef ot::ObjectNotFoundException ObjectNotFound; }

    //! @brief Basic object already exists exception.
    class ObjectAlreadyExistsException : public GeneralException {
    public:
        explicit ObjectAlreadyExistsException() : GeneralException("Object not found") {};
        explicit ObjectAlreadyExistsException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit ObjectAlreadyExistsException(const char* _message) : GeneralException(_message) {};
    };
    namespace Exception { typedef ot::ObjectAlreadyExistsException ObjectAlreadyExists; }

    //! @brief Basic out of bounds exception.
    class OutOfBoundsException : public GeneralException {
    public:
        explicit OutOfBoundsException() : GeneralException("Out of bounds") {};
        explicit OutOfBoundsException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit OutOfBoundsException(const char* _message) : GeneralException(_message) {};
    };
    namespace Exception { typedef ot::OutOfBoundsException OutOfBounds; }

    // ###########################################################################################################################################################################################################################################################################################################################

    // Network

    //! @brief Basic network error exception.
    class NetworkErrorException : public GeneralException {
    public:
        explicit NetworkErrorException() : GeneralException("Network error") {};
        explicit NetworkErrorException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit NetworkErrorException(const char* _message) : GeneralException(_message) {};
    };
    namespace Exception { typedef ot::NetworkErrorException NetworkError; }


}
