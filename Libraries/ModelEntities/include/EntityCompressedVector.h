// @otlicense
// File: EntityCompressedVector.h
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
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "OldTreeIcon.h"

#include <list>
#include <vector>

class __declspec(dllexport) EntityCompressedVector : public EntityBase
{
public:
	EntityCompressedVector() : EntityCompressedVector(0, nullptr, nullptr, nullptr, "") {};
	EntityCompressedVector(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityCompressedVector();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual void storeToDataBase(void) override;

	static std::string className() { return "EntityCompressedVector"; };
	virtual std::string getClassName(void) const override { return EntityCompressedVector::className(); };

	virtual entityType getEntityType(void) const override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

	void setTolerance(double tol);
	double getTolerance(void);
	
	void setConstantValue(double value, long long dimension);

	void setValues(const std::vector<double> &values);
	void getValues(std::vector<double> &values);

	void setValues(const double *values, size_t length);
	void getValues(double *values, size_t length);

	long long getUncompressedLength(void) { return uncompressedLength; }

	void clearData(void);

	bool multiply(std::vector<double> &values);
	bool add(std::vector<double> &values);

protected:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

private:
	void compressData(const double *values, size_t length, bool storeData, long long &compressedDataValuesSize, long long &compressedDataCountSize);

	const size_t			maximumDataSize;
	double					tolerance;
	long long				uncompressedLength;
	std::vector<double>		compressedDataValues;
	std::vector<long long>	compressedDataCount;
	long long				valuesSize;
	long long				countSize;
};


