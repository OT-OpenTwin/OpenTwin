//! @file FileSystem.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTSystem/SystemAPIExport.h"

// std header
#include <list>
#include <string>

namespace ot {

	class OT_SYS_API_EXPORT FileSystem {
		FileSystem() = delete;
	public:
		//! @brief Option flags used to change the behaviour of the FileSystem methods.
		enum FileSystemOption {
			NoOptions     = 0 << 0, //! @brief All options disabled.
			Recursive     = 1 << 0, //! @brief Search recursive.
			GenericFormat = 1 << 1  //! @brief Return paths in generic format (e.g. Root/Path even on windows).
		};
		typedef Flags<FileSystemOption> FileSystemOptions;

		//! @brief Returns all subdirectories at the given path.
		//! @param _path Top level path to search at.
		//! @param _options Search and format options.
		//! @throw std::filesystem::filesystem_error in case of filesystem related error.
		static std::list<std::string> getDirectories(const std::string& _path, const FileSystemOptions& _options = FileSystemOption::NoOptions);

		//! @brief Returns all files at the given path.
		//! @param _path Top level path to search at.
		//! @param _extensions Whitelisted file extensions. All files will be returned if the list is empty.
		//! @param _options Search and format options.
		//! @throw std::filesystem::filesystem_error in case of filesystem related error.
		static std::list<std::string> getFiles(const std::string& _path, const std::list<std::string>& _extensions, const FileSystemOptions& _options = FileSystemOption::NoOptions);

		//! @brief Reads a file line by line.
		//! @param _filePath File path.
		//! @throw ot::FileOpenException in case the file could not be opened.
		//! @throw std::ios_base::failure in case of serious IO error.
		static std::list<std::string> readLines(const std::string& _filePath);
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::FileSystem::FileSystemOption)