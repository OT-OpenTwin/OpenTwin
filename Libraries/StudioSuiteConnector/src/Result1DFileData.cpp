// @otlicense

#include "StudioSuiteConnector/Result1DFileData.h"

#include <fstream>
#include <cassert>

#include <qcryptographichash.h>

bool Result1DFileData::readData(const std::string& fileName)
{
    try
    {
        std::ifstream file(fileName, std::ios::binary);
        file.unsetf(std::ios::skipws);

        std::streampos file_size;
        file.seekg(0, std::ios::end);
        file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        bufferSize = file_size;
        buffer = new char[(size_t)file_size + 1];

        file.read(buffer, file_size);
        buffer[bufferSize - 1] = 0; // Add a string termination character

        calculateDataHash();

        return true;
    }
    catch (std::exception)
    {
        assert(0);
    }

    return false;
}

void Result1DFileData::calculateDataHash()
{
    // Calculate a hash value for the file data

    QCryptographicHash hashCalculator(QCryptographicHash::Md5);
    hashCalculator.addData(buffer, bufferSize);

    dataHashValue = hashCalculator.result().toHex().toStdString();
}


