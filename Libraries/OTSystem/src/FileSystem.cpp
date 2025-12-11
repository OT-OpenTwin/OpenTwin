// @otlicense
// File: FileSystem.cpp
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
#include "OTSystem/Exception.h"
#include "OTSystem/FileSystem.h"
#include <assert.h>
// std header
#include <filesystem>
#ifdef OT_OS_WINDOWS
    #include <Windows.h>
#else
    #include <sys/stat.h>
#endif

namespace ot {
    namespace intern {

        bool matchesExtension(const std::filesystem::path& _path, const std::list<std::string>& _extensions) {
            if (_extensions.empty()) {
                return true; // accept all if no filter
            }

            const std::string ext = _path.extension().string();
            for (const std::string& allowedExt : _extensions) {
                if (ext == allowedExt) {
                    return true;
                }
            }

            return false;
        }

    }
}

std::list<std::string> ot::FileSystem::getDirectories(const std::string& _path, const FileSystemOptions& _options) {
	std::list<std::string> result;

    if (_options & FileSystemOption::Recursive) {
        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(_path)) {
            if (entry.is_directory()) {
                if (_options & FileSystemOption::GenericFormat) {
                    result.push_back(entry.path().generic_string());
                }
                else {
                    result.push_back(entry.path().string());
                }
                
            }
        }
    }
    else {
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(_path)) {
            if (entry.is_directory()) {
                if (_options & FileSystemOption::GenericFormat) {
                    result.push_back(entry.path().generic_string());
                }
                else {
                    result.push_back(entry.path().string());
                }
            }
        }
    }

    return result;
}

ot::DateTime::Time ot::FileSystem::getLastAccessTime(const std::string& _path)
{
    ot::DateTime::Time accessTime;
#ifdef OT_OS_WINDOWS
    HANDLE handle = CreateFileA(_path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);
    FILETIME atime, mtime, ctime;
    GetFileTime(handle, &ctime, &atime, &mtime);

    SYSTEMTIME st;
    FileTimeToSystemTime(&atime, &st);
    accessTime.m_year = std::to_string(st.wYear);    
    accessTime.m_month = std::to_string(st.wMonth);
    accessTime.m_day = std::to_string(st.wDay);
#else
    assert(false); //Not tested yet
    struct stat info {};
    stat(_path.c_str(), &info);

    std::time_t atime = info.st_atime;   // last access
    struct tm* tm_info = localtime(&atime);

    accessTime.m_year = std::to_string(tm_info->tm_year + 1900);
    accessTime.m_month = std::to_string(tm_info->tm_mon + 1);
    accessTime.m_day = std::to_string(tm_info->tm_mday);
        
#endif
    return accessTime;
}

std::list<std::string> ot::FileSystem::getFiles(const std::string& _path, const std::list<std::string>& _extensions, const FileSystemOptions& _options) {
    std::list<std::string> result;

    if (_options & FileSystemOption::Recursive) {
        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(_path)) {
            if (entry.is_regular_file() && intern::matchesExtension(entry.path(), _extensions)) {
                if (_options & FileSystemOption::GenericFormat) {
                    result.push_back(entry.path().generic_string());
                }
                else {
                    result.push_back(entry.path().string());
                }
            }
        }
    }
    else {
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(_path)) {
            if (entry.is_regular_file() && intern::matchesExtension(entry.path(), _extensions)) {
                if (_options & FileSystemOption::GenericFormat) {
                    result.push_back(entry.path().generic_string());
                }
                else {
                    result.push_back(entry.path().string());
                }
            }
        }
    }

    return result;
}

std::list<std::string> ot::FileSystem::readLines(const std::string& _filePath, std::ifstream::iostate _exceptionMask) {
    std::list<std::string> lines;
    std::ifstream file(_filePath);

    if (!file.is_open()) {
        throw Exception::FileOpen("Failed to open fil for reading: \"" + _filePath + "\"");
    }

    file.exceptions(_exceptionMask);

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(std::move(line));
        line.clear();
    }

    return lines;
}
