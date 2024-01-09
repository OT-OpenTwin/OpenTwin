#pragma once
#include <stdint.h>

enum class InsertType : int32_t
{
	FileStorage =0,
	Database =1,
	JsonFileStorage = 2,
	GridFS = 3,
	GridFSAsc = 4
};