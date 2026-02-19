// @otlicense
// File: Exception.h
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

#pragma once

// std header
#include <string>
#include <exception>

#pragma warning (disable: 4251)

namespace ot {

    // ###########################################################################################################################################################################################################################################################################################################################

    // Base

	//! @brief Base class for all OpenTwin exceptions.
    class GeneralException : public std::exception {
    public:
        explicit GeneralException(const std::string& _message) : std::exception(_message.c_str()) {};
        explicit GeneralException(const char* _message) : std::exception(_message) {};
    };
    namespace Exception { typedef ot::GeneralException General; }

	//! @brief Basic null pointer exception.
	class NullPointerException : public GeneralException {
	public:
		explicit NullPointerException() : GeneralException("Null pointer") {};
		explicit NullPointerException(const char* _message) : GeneralException(_message) {};
		explicit NullPointerException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit NullPointerException(const std::string& _message, const std::string& _where) : NullPointerException("nullptr at \"" + _where + "\"" + (_message.empty() ? "" : ": " + _message)) {};
	};
	namespace Exception { typedef ot::NullPointerException NullPointer; }

    // ###########################################################################################################################################################################################################################################################################################################################

    // Data

	//! @brief Basic invalid format exception.
	class InvalidFormatException : public GeneralException {
	public:
		explicit InvalidFormatException() : GeneralException("Invalid format") {};
		explicit InvalidFormatException(const std::string& _message) : GeneralException(_message.c_str()) {};
		explicit InvalidFormatException(const char* _message) : GeneralException(_message) {};
	};
	namespace Exception { typedef ot::InvalidFormatException InvalidFormat; }

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

	//! @brief Basic argument missing exception.
	class ArgumentMissingException : public GeneralException {
	public:
		explicit ArgumentMissingException() : GeneralException("Argument missing") {};
		explicit ArgumentMissingException(const std::string& _message) : GeneralException(_message.c_str()) {};
		explicit ArgumentMissingException(const char* _message) : GeneralException(_message) {};
	};
	namespace Exception { typedef ot::ArgumentMissingException ArgumentMissing; }

    //! @brief Basic invalid argument exception.
    class InvalidArgumentException : public GeneralException {
    public:
        explicit InvalidArgumentException() : GeneralException("Invalid argument") {};
        explicit InvalidArgumentException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit InvalidArgumentException(const char* _message) : GeneralException(_message) {};
    };
    namespace Exception { typedef ot::InvalidArgumentException InvalidArgument; }

	//! @brief Basic invalid type exception.
	class InvalidTypeException : public GeneralException {
	public:
		explicit InvalidTypeException() : GeneralException("Invalid type") {};
		explicit InvalidTypeException(const std::string& _message) : GeneralException(_message.c_str()) {};
		explicit InvalidTypeException(const char* _message) : GeneralException(_message) {};
	};
	namespace Exception { typedef ot::InvalidTypeException InvalidType; }

	//! @brief Basic invalid version exception.
	class InvalidVersionException : public GeneralException {
	public:
		explicit InvalidVersionException() : GeneralException("Invalid version") {};
		explicit InvalidVersionException(const std::string& _message) : GeneralException(_message.c_str()) {};
		explicit InvalidVersionException(const char* _message) : GeneralException(_message) {};
	};
	namespace Exception { typedef ot::InvalidVersionException InvalidVersion; }

    // ###########################################################################################################################################################################################################################################################################################################################

    // Runtime

    //! @brief Basic runtime exception.
    class RuntimeException : public GeneralException {
    public:
        explicit RuntimeException() : GeneralException("Runtime exception") {};
        explicit RuntimeException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit RuntimeException(const char* _message) : GeneralException(_message) {};
    };
    namespace Exception { typedef ot::RuntimeException Runtime; }

	//! @brief Basic serialization exception.
	class SerializationException : public RuntimeException {
	public:
		explicit SerializationException() : RuntimeException("Serialization exception") {};
		explicit SerializationException(const std::string& _message) : RuntimeException(_message.c_str()) {};
		explicit SerializationException(const char* _message) : RuntimeException(_message) {};
	};
	namespace Exception { typedef ot::SerializationException Serialization; }

	//! @brief Basic deserialization exception.
	class DeserializationException : public RuntimeException {
	public:
		explicit DeserializationException() : RuntimeException("Deserialization exception") {};
		explicit DeserializationException(const std::string& _message) : RuntimeException(_message.c_str()) {};
		explicit DeserializationException(const char* _message) : RuntimeException(_message) {};
	};
	namespace Exception { typedef ot::DeserializationException Deserialization; }

    // ###########################################################################################################################################################################################################################################################################################################################

	// State

    //! @brief Basic illegal state exception.
    class IllegalStateException : public GeneralException {
    public:
        explicit IllegalStateException() : GeneralException("Illegal state") {};
        explicit IllegalStateException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit IllegalStateException(const char* _message) : GeneralException(_message) {};
    };
    namespace Exception { typedef ot::IllegalStateException IllegalState; }

	//! @brief Basic not initialized exception.
    class NotInitializedException : public IllegalStateException {
    public:
        explicit NotInitializedException() : IllegalStateException("Not initialized") {};
        explicit NotInitializedException(const std::string& _message) : IllegalStateException(_message.c_str()) {};
        explicit NotInitializedException(const char* _message) : IllegalStateException(_message) {};
    };
	namespace Exception { typedef ot::NotInitializedException NotInitialized; }

	//! @brief Basic already initialized exception.
	class AlreadyInitializedException : public IllegalStateException {
	public:
		explicit AlreadyInitializedException() : IllegalStateException("Already initialized") {};
		explicit AlreadyInitializedException(const std::string& _message) : IllegalStateException(_message.c_str()) {};
		explicit AlreadyInitializedException(const char* _message) : IllegalStateException(_message) {};
	};
	namespace Exception { typedef ot::AlreadyInitializedException AlreadyInitialized; }

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

    // Configuration

    //! @brief Basic configuration exception.
    class ConfigurationException : public GeneralException {
    public:
        explicit ConfigurationException() : GeneralException("Configuration exception") {};
        explicit ConfigurationException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit ConfigurationException(const char* _message) : GeneralException(_message) {};
    };
    namespace Exception { typedef ot::ConfigurationException Configuration; }

    //! @brief Basic configuration not found exception.
    class ConfigurationNotFoundException : public ConfigurationException {
    public:
        explicit ConfigurationNotFoundException() : ConfigurationException("Configuration not found") {};
        explicit ConfigurationNotFoundException(const std::string& _message) : ConfigurationException(_message.c_str()) {};
        explicit ConfigurationNotFoundException(const char* _message) : ConfigurationException(_message) {};
    };
    namespace Exception { typedef ot::ConfigurationNotFoundException ConfigurationNotFound; }

    class InvalidConfigurationException : public ConfigurationException {
    public:
        explicit InvalidConfigurationException() : ConfigurationException("Invalid configuration") {};
        explicit InvalidConfigurationException(const std::string& _message) : ConfigurationException(_message.c_str()) {};
        explicit InvalidConfigurationException(const char* _message) : ConfigurationException(_message) {};
    };
    namespace Exception { typedef ot::InvalidConfigurationException InvalidConfiguration; }

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

    // ###########################################################################################################################################################################################################################################################################################################################

    // Other

	//! @brief Basic script exception.
	class ScriptException : public GeneralException {
        public:
        explicit ScriptException() : GeneralException("Script exception") {};
        explicit ScriptException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit ScriptException(const char* _message) : GeneralException(_message) {};
	};
	namespace Exception { typedef ot::ScriptException Script; }

	//! @brief Basic graphics exception.
	class GraphicsException : public GeneralException {
        public:
        explicit GraphicsException() : GeneralException("Graphics exception") {};
        explicit GraphicsException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit GraphicsException(const char* _message) : GeneralException(_message) {};
	};
	namespace Exception { typedef ot::GraphicsException Graphics; }

	//! @brief Basic thread exception.
	class ThreadException : public GeneralException {
        public:
        explicit ThreadException() : GeneralException("Thread exception") {};
        explicit ThreadException(const std::string& _message) : GeneralException(_message.c_str()) {};
        explicit ThreadException(const char* _message) : GeneralException(_message) {};
	};
	namespace Exception { typedef ot::ThreadException Thread; }
}
