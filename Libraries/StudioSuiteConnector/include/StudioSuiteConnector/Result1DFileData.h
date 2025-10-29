// @otlicense

#pragma once

#include <string>

class Result1DFileData
{
public:
	Result1DFileData() : buffer(nullptr), bufferSize(0) {};
	~Result1DFileData() { if (buffer != nullptr) delete[] buffer; buffer = nullptr; };

	bool readData(const std::string& fileName);

	size_t getBufferSize() { return bufferSize; }
	char* getBuffer() { return buffer; }

	std::string getDataHash() { return dataHashValue; }

private:
	void calculateDataHash();

	char* buffer;
	size_t bufferSize;
	std::string dataHashValue;
};
