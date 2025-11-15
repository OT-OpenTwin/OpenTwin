// @otlicense
// File: EntityVis2D3D.h
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

#include "EntityContainer.h"
#include "EntityResultBase.h"

#include <list>

class __declspec(dllexport) EntityVis2D3D : public EntityContainer
{
public:
	EntityVis2D3D() : EntityVis2D3D(0, nullptr, nullptr, nullptr) {};
	EntityVis2D3D(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityVis2D3D();

	virtual void addVisualizationNodes(void) override;

	virtual std::string getClassName(void) const override { return "EntityVis2D3D"; } ;

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual bool updateFromProperties(void) override;

	virtual void createProperties(void);
	virtual bool updatePropertyVisibilities(void);

	ot::UID getDataID(void) { return visualizationDataID; }
	void setDataID(ot::UID data) { visualizationDataID = data; setModified(); }

	ot::UID getDataVersion(void) { return visualizationDataVersion; }
	void setDataVersion(ot::UID data) { visualizationDataVersion = data; setModified(); }

	void setResultType(EntityResultBase::tResultType type) { resultType = type; }
	EntityResultBase::tResultType getResultType(void) { return resultType; }

	void setSource(ot::UID id, ot::UID version) { sourceID = id; sourceVersion = version; }
	void setMesh(ot::UID id, ot::UID version) { meshID = id; meshVersion = version; }

	//void ensureSourceDataLoaded(void);

	ot::UID getSourceID(void) { return sourceID; }
	ot::UID getSourceVersion(void) { return sourceVersion; }

	ot::UID getMeshID(void) { return meshID; }
	ot::UID getMeshVersion(void) { return meshVersion; }

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

protected:
	ot::UID visualizationDataID;
	ot::UID visualizationDataVersion;
	ot::UID sourceID;
	ot::UID sourceVersion;
	ot::UID meshID;
	ot::UID meshVersion;
	EntityResultBase::tResultType resultType;

	// Temporary
	//EntityResultBase *source;
};



