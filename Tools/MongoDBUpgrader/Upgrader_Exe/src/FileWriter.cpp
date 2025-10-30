// @otlicense
// File: FileWriter.cpp
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

#include "FileWriter.h"

FileWriter::FileWriter(const std::string& _filePath, bool _append)
{
    if (_append)
    {
        m_fStream = std::fstream(_filePath, std::ios::app);
    }
    else
    {
        m_fStream = std::fstream(_filePath, std::ios::out);
    }
    if (!m_fStream.is_open())
    {
        throw std::exception(std::string("Failed to open file: " + _filePath).c_str());
    }
}

void FileWriter::write(const std::string& _content)
{

    std::string line = _content;
    if (line.back() != '\n')
    {
        line += "\n";
    }
    m_fStream.write(line.c_str(), line.size());
}

void FileWriter::close()
{
    m_fStream.close();
}

FileWriter::~FileWriter()
{
    if (m_fStream.is_open())
    {
        m_fStream.flush();
        close();
    }
}
