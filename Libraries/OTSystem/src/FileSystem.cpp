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
#include "OTSystem/OTAssert.h"
#include "OTSystem/Exception.h"
#include "OTSystem/FileSystem.h"

// std header
#include <string>
#include <vector>
#include <sstream>
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

bool ot::FileSystem::readFile(const std::string& _filePath, std::string& _outputContent, uint64_t& _fileSize, std::ifstream::iostate _exceptionMask) {
    _outputContent.clear();
    _fileSize = 0;

    std::ifstream file;
    file.exceptions(_exceptionMask);

    try {
        // Open file
        file.open(_filePath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return false;
        }

        // Determine file size
        const std::ifstream::pos_type pos = file.tellg();
        if (pos == std::ifstream::pos_type(-1)) {
            return false;
        }

        const std::streamoff sizeOff = static_cast<std::streamoff>(pos);
        if (sizeOff < 0) {
            return false;
        }

        _fileSize = static_cast<uint64_t>(sizeOff);

        // Guard against std::string size overflow
        if (_fileSize > _outputContent.max_size()) {
            return false;
        }

        // Read file
        _outputContent.resize(static_cast<std::size_t>(_fileSize));
        file.seekg(0, std::ios::beg);

        if (_fileSize > 0) {
            file.read(&_outputContent[0], static_cast<std::streamsize>(_fileSize));
            if (!file) {
                _outputContent.clear();
                _fileSize = 0;
                return false;
            }
        }

        return true;
    }
    catch (const std::ios_base::failure&) {
        _outputContent.clear();
        _fileSize = 0;
        return false;
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
ot::DateTime ot::FileSystem::getLastModifiedTime(const std::string& _path, bool _useLocalTime) {
    DateTime modifiedTime;
#ifdef OT_OS_WINDOWS
    HANDLE handle = CreateFileA(_path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);

    if (handle == INVALID_HANDLE_VALUE) {
        return DateTime();
    }

    FILETIME atime, mtime, ctime;
    GetFileTime(handle, &ctime, &atime, &mtime);

    SYSTEMTIME st;

    if (_useLocalTime) {
        FILETIME localFt;
        FileTimeToLocalFileTime(&mtime, &localFt);
        FileTimeToSystemTime(&localFt, &st);
    }
    else {
        FileTimeToSystemTime(&mtime, &st);
    }

    modifiedTime = DateTime(
        static_cast<int>(st.wYear),
        static_cast<int>(st.wMonth),
        static_cast<int>(st.wDay),
        static_cast<int>(st.wHour),
        static_cast<int>(st.wMinute),
        static_cast<int>(st.wSecond),
        static_cast<int>(st.wMilliseconds)
    );

    CloseHandle(handle);

#else
    struct stat info {};

    if (stat(_path.c_str(), &info) != 0) {
        return DateTime();
    }

    std::time_t mtime = info.st_mtime;
    struct tm tmResult;

    if (_useLocalTime) {
        localtime_r(&mtime, &tmResult);
    }
    else {
        gmtime_r(&mtime, &tmResult);
    }

    long millisec = 0;

#if defined(OT_OS_MAC)
    millisec = info.st_mtimespec.tv_nsec / 1000000;
#else
    millisec = info.st_mtim.tv_nsec / 1000000;
#endif

    modifiedTime = DateTime(
        tmResult.tm_year + 1900,
        tmResult.tm_mon + 1,
        tmResult.tm_mday,
        tmResult.tm_hour,
        tmResult.tm_min,
        tmResult.tm_sec,
        static_cast<int>(millisec)
    );
#endif
    return modifiedTime;
}


ot::DateTime ot::FileSystem::getLastAccessTime(const std::string& _path, bool _useLocalTime) {
    DateTime accessTime;
#ifdef OT_OS_WINDOWS
    HANDLE handle = CreateFileA(_path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);

    if (handle == INVALID_HANDLE_VALUE) {
        return DateTime();
    }

    FILETIME atime, mtime, ctime;
    GetFileTime(handle, &ctime, &atime, &mtime);

    SYSTEMTIME st;

    if (_useLocalTime) {
        FILETIME localFt;
        FileTimeToLocalFileTime(&atime, &localFt);
        FileTimeToSystemTime(&localFt, &st);
    }
    else {
        FileTimeToSystemTime(&atime, &st);
    }

    accessTime = DateTime(
        static_cast<int>(st.wYear),
        static_cast<int>(st.wMonth),
        static_cast<int>(st.wDay),
        static_cast<int>(st.wHour),
        static_cast<int>(st.wMinute),
        static_cast<int>(st.wSecond),
        static_cast<int>(st.wMilliseconds)
    );

    CloseHandle(handle);

#else
    struct stat info {};

    if (stat(_path.c_str(), &info) != 0) {
        return DateTime();
    }

    std::time_t atime = info.st_atime;
    struct tm tmResult;

    if (_useLocalTime) {
        localtime_r(&atime, &tmResult);
    }
    else {
        gmtime_r(&atime, &tmResult);
    }

    long millisec = 0;

#if defined(OT_OS_MAC)
    millisec = info.st_atimespec.tv_nsec / 1000000;
#else
    millisec = info.st_atim.tv_nsec / 1000000;
#endif

    accessTime = DateTime(
        tmResult.tm_year + 1900,
        tmResult.tm_mon + 1,
        tmResult.tm_mday,
        tmResult.tm_hour,
        tmResult.tm_min,
        tmResult.tm_sec,
        static_cast<int>(millisec)
    );
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

std::string ot::FileSystem::toFileSizeString(uint64_t _bytes, int _precision) {
    static std::vector<std::string> units {
        "B", "KB", "MB", "GB", "TB", "PB", "EB"
    };

    if (_bytes == 0) {
        return "0B";
    }

    double value = static_cast<double>(_bytes);
    size_t unitIndex = 0;

    while (value >= 1024.0 && unitIndex + 1 < units.size()) {
        value /= 1024.0;
        unitIndex++;
    }

    std::ostringstream stream;
    stream.imbue(std::locale::classic());
    stream << std::fixed << std::setprecision(_precision) << value;

    std::string result = stream.str();

    // Remove trailing zeros and optional decimal point
    if (_precision > 0) {
        result.erase(result.find_last_not_of('0') + 1);
        if (!result.empty() && result.back() == '.') {
            result.pop_back();
        }
    }

	OTAssert(unitIndex >= 0 && unitIndex < units.size(), "Unit index out of range");
    result.append(units[unitIndex]);
    return result;
}