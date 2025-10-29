// @otlicense

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
