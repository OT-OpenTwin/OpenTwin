// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Matrix2.h"

// std header
#include <vector>

template <class T>
class Matrix2Factory {
public:
	static Matrix2<T>* createRowWiseOptimized(int64_t _rows, int64_t _columns, std::vector<std::vector<T>> _columnWiseData);
	static Matrix2<T>* createRowWiseOptimizedEmpty(int64_t _rows, int64_t _columns);
	static Matrix2<T>* createColumnWiseOptimized(int64_t _rows, int64_t _columns, std::vector<std::vector<T>> _columnWiseData);
	static Matrix2<T>* createColumnWiseOptimizedEmpty(int64_t _rows, int64_t _columns);
};

#include "OTCore/Matrix2Factory.hpp"