//#include "FixtureMatrix.h"
//#include "OpenTwinFoundation/MatrixFactory.h"
//
//FixtureMatrix::FixtureMatrix()
//{
//	_rows = 5;
//	_columns = 3;
//	_expectedDataInt.reserve(_columns);
//
//	for (int j = 0; j < _columns; j++)
//	{
//		std::vector<int> temp(_rows);
//
//		for (int i = 0; i < _rows; i++)
//		{
//			int index = _rows * i + j * _columns;
//			temp[i] = index;
//		}
//		_expectedDataInt.push_back(temp);
//	}	
//}
//
//Matrix<int>* FixtureMatrix::CreateSquareMatrixRowWiseOptimizedInt()
//{
//	return MatrixFactory<int>::CreateRowWiseOptimized(_rows, _columns, _expectedDataInt);
//}