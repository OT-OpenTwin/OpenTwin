// @otlicense
// File: EntityResultTableData.h
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

#include <list>
#include <vector>

template <class T>
class __declspec(dllexport) EntityResultTableData : public EntityBase
{
public:
	EntityResultTableData() : EntityResultTableData(0, nullptr, nullptr, nullptr) {};
	EntityResultTableData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityResultTableData();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual void storeToDataBase(void) override;

	static std::string classNameBase() { return "EntityResultTableData"; };
	virtual std::string getClassName(void) const override { return className; };

	virtual entityType getEntityType(void) const override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

	void setNumberOfColumns(uint64_t cols);
	void setNumberOfRows(uint64_t rows);

	void setHeaderText(unsigned int col, const std::string &text);
	void setValue(unsigned int row, unsigned int col, T value);

	void setRow(unsigned int row, std::vector<T>& value);
	void setColumn(unsigned int column, std::vector<T>& value);

	uint64_t getNumberOfRows(void);
	uint64_t getNumberOfColumns(void);
	std::string getHeaderText(unsigned int col);
	T getValue(unsigned int row, unsigned int col);

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	
	std::string className;
	std::vector<std::string> header;
	std::vector<std::vector<T>> data;
};

#include "EntityResultTableData.hpp"


