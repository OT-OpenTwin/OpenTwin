//! @file Matrix2Factory.h
//! @author Wagner
//! @date February 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Matrix2Factory.h"

template<class T>
inline Matrix2<T>* Matrix2Factory<T>::createRowWiseOptimized(int64_t rows, int64_t columns, std::vector<std::vector<T>> columnWiseData) {
	auto temp = new Matrix2<T>(rows, columns, Matrix2<T>::accessOptimizationDirection::rowWiseAccess);
	for (int col = 0; col < columns; col++) {
		for (int row = 0; row < rows; row++) {
			temp->SetValue(row, col, columnWiseData[col][row]);
		}
	}

	return temp;
}

template<class T>
inline Matrix2<T>* Matrix2Factory<T>::createRowWiseOptimizedEmpty(int64_t rows, int64_t columns) {
	return new Matrix2<T>(rows, columns, Matrix2<T>::accessOptimizationDirection::rowWiseAccess);
}

template<class T>
inline Matrix2<T>* Matrix2Factory<T>::createColumnWiseOptimized(int64_t rows, int64_t columns, std::vector<std::vector<T>> columnWiseData) {
	auto temp = new Matrix2<T>(rows, columns, Matrix2<T>::accessOptimizationDirection::columnWiseAccess);
	for (int col = 0; col < columns; col++) {
		for (int row = 0; row < rows; row++) {
			temp->SetValue(row, col, columnWiseData[col][row]);
		}
	}

	return temp;
}

template<class T>
inline Matrix2<T>* Matrix2Factory<T>::createColumnWiseOptimizedEmpty(int64_t rows, int64_t columns) {
	return new Matrix2<T>(rows, columns, Matrix2<T>::accessOptimizationDirection::columnWiseAccess);
}

