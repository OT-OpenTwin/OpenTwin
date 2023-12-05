#pragma once
#include "MatrixFactory.h"


template<class T>
inline Matrix<T> * MatrixFactory<T>::CreateRowWiseOptimized(int64_t rows, int64_t columns, std::vector<std::vector<T>> columnWiseData)
{
	auto temp = new Matrix<T>(rows, columns, Matrix<T>::accessOptimizationDirection::rowWiseAccess);
	for (int col = 0; col < columns;col++)
	{
		for (int row = 0; row < rows; row++)
		{
			temp->SetValue(row, col, columnWiseData[col][row]);
		}
	}

	return temp;
}

template<class T>
inline Matrix<T> * MatrixFactory<T>::CreateRowWiseOptimizedEmpty(int64_t rows, int64_t columns)
{
	return new Matrix<T>(rows, columns, Matrix<T>::accessOptimizationDirection::rowWiseAccess);
}

template<class T>
inline Matrix<T> * MatrixFactory<T>::CreateColumnWiseOptimized(int64_t rows, int64_t columns, std::vector<std::vector<T>> columnWiseData)
{
	auto temp = new Matrix<T>(rows, columns, Matrix<T>::accessOptimizationDirection::columnWiseAccess);
	for (int col = 0; col < columns;col++)
	{
		for (int row = 0; row < rows; row++)
		{
			temp->SetValue(row, col, columnWiseData[col][row]);
		}
	}

	return temp;
}

template<class T>
inline Matrix<T> * MatrixFactory<T>::CreateColumnWiseOptimizedEmpty(int64_t rows, int64_t columns)
{
	return new Matrix<T>(rows, columns, Matrix<T>::accessOptimizationDirection::columnWiseAccess);
}

