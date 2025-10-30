// @otlicense
// File: EntityResultTable.h
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
#include "EntityResultTableData.h"

#include <memory>

template <class T>
class __declspec(dllexport) EntityResultTable : public EntityBase
{
public:
	EntityResultTable() : EntityResultTable(0, nullptr, nullptr, nullptr, "") {};
	EntityResultTable(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, const std::string &owner);
	virtual ~EntityResultTable();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;
	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };
	virtual void storeToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	void addVisualizationItem(bool isHidden);
	virtual std::string getClassName(void) const override { return className; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void createProperties(void);
	virtual bool updateFromProperties(void) override;

	void deleteTableData(void);
	void releaseTableData(void);
	long long getTableDataStorageId(void) { return tableDataStorageId; }
	long long getTableDataStorageVersion(void) { return tableDataStorageVersion; }
	std::shared_ptr<EntityResultTableData<T>> getTableData(void);
	void setTableData(std::shared_ptr<EntityResultTableData<T>> tableData);

	int getMinColumn();
	int getMinRow();
	int getMaxColumn();
	int getMaxRow();

	void setMinColumn(int minCol);
	void setMinRow(int minRow);
	void setMaxColumn(int maxCol);
	void setMaxRow(int maxRow);

protected:
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	int _minCol = 0;
	int _minRow = 1;

private:
	int _maxCol = 50;
	int _maxRow = 50;
	virtual int getSchemaVersion(void) override { return 1; };
	void ensureTableDataLoaded(void);
	std::string className; 
	std::shared_ptr<EntityResultTableData<T>> tableData = nullptr;
	long long tableDataStorageId;
	long long tableDataStorageVersion;
};

#include "EntityResultTable.hpp"