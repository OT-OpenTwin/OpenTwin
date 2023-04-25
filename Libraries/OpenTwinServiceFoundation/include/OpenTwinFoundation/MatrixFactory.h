#pragma once
#include <vector>
#include "Matrix.h"
template <class T>
class MatrixFactory
{
public:
	static Matrix<T>* CreateRowWiseOptimized(int64_t rows, int64_t columns, std::vector<std::vector<T>> columnWiseData);
	static Matrix<T>* CreateRowWiseOptimizedEmpty(int64_t rows, int64_t columns);
	static Matrix<T>* CreateColumnWiseOptimized(int64_t rows, int64_t columns, std::vector<std::vector<T>> columnWiseData);
	static Matrix<T>* CreateColumnWiseOptimizedEmpty(int64_t rows, int64_t columns);
};


#include "MatrixFactory.hpp"