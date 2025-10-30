// @otlicense
// File: Dataset.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once
#ifndef DATASET_CLASS

#include <iostream>
#include <vector>
#include <list>
#include <array>
#include <string>

using namespace std;

//
//Nx, Ny
//
//x1, x2, x3, ..., xNx, y1, y2, ..., yNy
//
//setNumberParameters(int Nx, int Ny);  // Delete all storage
//addDataPoint(std::vector<double> xValues, std::vector<double> yValues);
//Check if xValues.size() == Nx ...
//Create Ny vectors : x1, x2, ..., xNx, yi
//Add vectors to large storage field
//
//std::vector<std::list<std::vector<double>>> storageForAllY
//
//

class DataSet
{
private:
	long nX;
	long nY;
	std::list<std::vector<double>> xValues;
	std::list<std::vector<double>> yValues;
	std::vector<double> yPredictions;

public:
	void setDataSize(long nX, long nY) {
		this->nX = nX;
		this->nY = nY;
		this->xValues = {};
		this->yValues = {};
		this->yPredictions = {};
	}
	std::array<long, 2> getDataSize() {
		std::array<long, 2> size = { this->nX, this->nY };
		return size;
	}

	void addDataPoints(std::vector<double> xValues, std::vector<double> yValues) {
		if (!(this->nX == xValues.size() && this->nY == yValues.size()))
		{
			cout << "data size doesn't match\t:[" << nX << ", " << nY << "] != [" << xValues.size() << ", " << yValues.size() << "]\n";
			return;
		};

		this->xValues.push_back(xValues);
		this->yValues.push_back(yValues);
	}

	std::list<std::vector<double>> getxDataPoints() {
		return this->xValues;
	}
	std::list<std::vector<double>> getyDataPoints() {
		return this->yValues;
	}
	std::vector<double> getyPredictionPoints() {
		return this->yPredictions;
	}

	void printDataSet() {
		std::cout << "\nprinting dataset\n";
		std::cout << "---------------------------------------------------------------\n";
		int i = 0;

		std::list<std::vector<double>>::iterator it = this->yValues.begin();
		for (const std::vector<double> & row : this->xValues)
		{
			for (int j{}; j < row.size(); j++) {
				std::cout << row.at(j) << "\t";
			}
			cout << "| ";

			const std::vector<double> yrow = *it;
			for (int j{}; j < yrow.size(); j++) {
				std::cout << yrow.at(j) << "\t";
			}
			it = std::next(it, 1);
			std::cout << std::endl;

			i = i + 1;
		}
		std::cout << "---------------------------------------------------------------\n";
	}

	void printxValues() {
		cout << "\n\nprinting x values...\n";

		for (const std::vector<double> & row : this->xValues)
		{
			int i = 0;
			for (int j{}; j < row.size(); j++) {
				cout << row.at(j) << "\t";
			}
			cout << endl;
			i++;
		}

	}

	static std::string toString(std::vector<double> data) {
		std::string ret = "";

		for (const double& val : data)
		{
			ret.append(std::to_string(val)).append("\t");
		}

		return ret;
	}

	static std::string toString(std::list<std::vector<double>> data) {
		std::string ret = "";

		std::list<std::vector<double>>::iterator it = data.begin();
		for (const std::vector<double> & row : data)
		{
			for (int j{}; j < row.size(); j++) {
				double val = row.at(j);
				ret.append(std::to_string(val)).append("\t");
			}
			ret.append("\n");
		}

		return ret;
	}


	template <typename T, size_t n>
	void findSize(T(&arr)[n])
	{
		std::cout << sizeof(T) * n << endl;
	}

	static DataSet prepareDataset() {
		cout << "generating dataset ...\n";
		const int ROW_SIZE = 10;
		const int COL_SIZE = 10;
		const int Y_COL_SIZE = 1;

		DataSet ds;
		ds.setDataSize(COL_SIZE, Y_COL_SIZE);

		for (unsigned int i = 0; i < ROW_SIZE; i++) {

			std::vector<double> xValue;
			for (unsigned int j = 0; j < COL_SIZE; j++) {
				double v = i * ROW_SIZE + j;
				xValue.push_back(v);
			}

			std::vector<double> yValue;
			for (unsigned int j = 0; j < Y_COL_SIZE; j++) {
				double v = (j + 1) * 10 + i + 0.1 * j;
				yValue.push_back(v);
			}

			ds.addDataPoints(xValue, yValue);
		}
		cout << endl;

		return ds;
	}
};

#endif