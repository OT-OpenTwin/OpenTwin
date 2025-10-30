// @otlicense

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
    namespace Exception { typedef ot::GeneralException General; }

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

    //! @brief Basic invalid argument exception.
    class InvalidArgumentException : public GeneralException {
    public:
        explicit InvalidArgumentException() : GeneralException("Invalid argument") {};
        explicit InvalidArgumentException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit InvalidArgumentException(const char* _message) : GeneralException(_message) {};
    };
    namespace Exception { typedef ot::InvalidArgumentException InvalidArgument; }

    // ###########################################################################################################################################################################################################################################################################################################################

    // Files

    //! @brief Basic file exception.
    class FileException : public GeneralException {
    public:
        explicit FileException() : GeneralException("File exception") {};
        explicit FileException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit FileException(const char* _message) : GeneralException(_message) {};
    };
    namespace Exception { typedef ot::FileException File; }

    //! @brief Basic file open exception.
    class FileOpenException : public FileException {
    public:
        explicit FileOpenException() : FileException("File open exception") {};
        explicit FileOpenException(const std::string& _message) : FileException(_message) {};
        explicit FileOpenException(const char* _message) : FileException(_message) {};
    };
    namespace Exception { typedef ot::FileOpenException FileOpen; }

    //! @brief Basic file read exception.
    class FileReadException : public FileException {
    public:
        explicit FileReadException() : FileException("File read exception") {};
        explicit FileReadException(const std::string& _message) : FileException(_message) {};
        explicit FileReadException(const char* _message) : FileException(_message) {};
    };
    namespace Exception { typedef ot::FileReadException FileRead; }

    //! @brief Basic file write exception.
    class FileWriteException : public FileException {
    public:
        explicit FileWriteException() : FileException("File write exception") {};
        explicit FileWriteException(const std::string& _message) : FileException(_message) {};
        explicit FileWriteException(const char* _message) : FileException(_message) {};
    };
    namespace Exception { typedef ot::FileWriteException FileWrite; }

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

    //! @brief Basic request failed exception.
    class RequestFailedException : public GeneralException {
    public:
        explicit RequestFailedException() : GeneralException("Request failed") {};
        explicit RequestFailedException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit RequestFailedException(const char* _message) : GeneralException(_message) {};
    };
    namespace Exception { typedef ot::RequestFailedException RequestFailed; }

}
