#pragma once


class FileHandler
{
public:
	FileHandler() = default;
	~FileHandler() = default;

	FileHandler(const FileHandler& _other) = delete;
	FileHandler(FileHandler&& _other) = delete;
	FileHandler& operator=(const FileHandler& _other) = delete;
	FileHandler& operator=(FileHandler&& _other) = delete;

private:


};
