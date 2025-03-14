#pragma once
#include <string>
#include <fstream>

class FileWriter
{
public:
    FileWriter(const std::string& _filePath, bool _append = false);
    ~FileWriter();
    
    void write(const std::string& _content);
    void close();

private:
    std::fstream m_fStream;
};
