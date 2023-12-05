#pragma once
#include "DataObject.h"
#include <memory>
#include <vector>

template <class T>
class Tensor : public DataObject
{
public:
	Tensor(std::vector<uint32_t>& dimensions, uint32_t memoryOptimizedDimension);
	Tensor(std::vector<uint32_t>&& dimensions, uint32_t memoryOptimizedDimension);

private:
	std::vector<uint32_t> _dimensions;
	T* _data = nullptr;

	enum Alignment { CacheLine64 = 64, AVX = 128, AVX2 = 256, AVX512 = 512 };
	int _defaultAlignment = Alignment::CacheLine64;
};
