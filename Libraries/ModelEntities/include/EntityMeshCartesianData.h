// @otlicense
// File: EntityMeshCartesianData.h
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

#include <list>
#include <string>
#include <vector>

class EntityMeshCartesianFaceList;
class EntityMeshCartesianNodes;
class EntityCartesianVector;

class __declspec(dllexport) EntityMeshCartesianData : public EntityContainer
{
public:
	EntityMeshCartesianData() : EntityMeshCartesianData(0, nullptr, nullptr, nullptr) {};
	EntityMeshCartesianData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityMeshCartesianData();

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual bool updateFromProperties(void) override;

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual void storeToDataBase(void) override;

	virtual void addVisualizationNodes(void) override;
	void addVisualizationItem(bool isHidden);

	virtual std::string getClassName(void) const override { return "EntityMeshCartesianData"; }

	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual void removeChild(EntityBase *child) override;

	size_t getNumberLinesX(void) { return meshCoords[0].size();	}
	size_t getNumberLinesY(void) { return meshCoords[1].size(); }
	size_t getNumberLinesZ(void) { return meshCoords[2].size(); }

	size_t getNumberCells(void) { return getNumberLinesX() * getNumberLinesY() * getNumberLinesZ(); }

	std::vector<double> &getMeshLinesX(void) { return  meshCoords[0]; }
	std::vector<double> &getMeshLinesY(void) { return  meshCoords[1]; }
	std::vector<double> &getMeshLinesZ(void) { return  meshCoords[2]; }

	double getSmallestStepWidth(void);
	double getLargestStepWidth(void);
	double getStepRatio(void);

	size_t findLowerIndex(int direction, double value);
	size_t findUpperIndex(int direction, double value);

	EntityMeshCartesianFaceList *getMeshFaces(void);
	long long getMeshFacesStorageId(void) { return meshFacesStorageId; }

	EntityMeshCartesianNodes *getMeshNodes(void);
	long long getMeshNodesStorageId(void) { return meshNodesStorageId; }

	void storeMeshFaces(void);
	void releaseMeshFaces(void);

	void storeMeshNodes(void);
	void releaseMeshNodes(void);

	void setDsMatrix(EntityCartesianVector *matrix);
	void setDualDsMatrix(EntityCartesianVector *matrix);
	void setDaMatrix(EntityCartesianVector *matrix);
	void setDualDaMatrix(EntityCartesianVector *matrix);
	void setDepsMatrix(EntityCartesianVector *matrix);
	void setDmuMatrix(EntityCartesianVector *matrix);
	void setDsigmaMatrix(EntityCartesianVector *matrix);

	bool applyDsMatrix(std::vector<double> &vector, bool keepMatrixInMemory);
	bool applyDualDsMatrix(std::vector<double> &vector, bool keepMatrixInMemory);
	bool applyDaMatrix(std::vector<double> &vector, bool keepMatrixInMemory);
	bool applyDualDaMatrix(std::vector<double> &vector, bool keepMatrixInMemory);
	bool applyDepsMatrix(std::vector<double> &vector, bool keepMatrixInMemory);
	bool applyDmuMatrix(std::vector<double> &vector, bool keepMatrixInMemory);
	bool applyDsigmaMatrix(std::vector<double> &vector, bool keepMatrixInMemory);

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void EnsureFacesLoaded(void);
	void EnsureNodesLoaded(void);
	void EnsureMatrixLoaded(EntityCartesianVector *&matrix, long long storageId, long long storageVersion);
	long long getStorageId(bsoncxx::document::view &doc_view, const std::string dataName);
	EntityCartesianVector* getDsMatrix(void);
	EntityCartesianVector* getDualDsMatrix(void);
	EntityCartesianVector* getDaMatrix(void);
	EntityCartesianVector* getDualDaMatrix(void);
	EntityCartesianVector* getDepsMatrix(void);
	EntityCartesianVector* getDmuMatrix(void);
	EntityCartesianVector* getDsigmaMatrix(void);
	bool prepareMatrixApplication(std::vector<double> &vector, bool keepMatrixInMemory, EntityCartesianVector *matrix, EntityCartesianVector *&loadedMatrix);

	std::vector<double> meshCoords[3];

	EntityMeshCartesianFaceList *meshFaces;
	long long meshFacesStorageId;
	long long meshFacesStorageVersion;

	EntityMeshCartesianNodes *meshNodes;
	long long meshNodesStorageId;
	long long meshNodesStorageVersion;

	EntityCartesianVector *matrixDs;
	EntityCartesianVector *matrixDualDs;
	EntityCartesianVector *matrixDa;
	EntityCartesianVector *matrixDualDa;
	EntityCartesianVector *matrixDeps;
	EntityCartesianVector *matrixDmu;
	EntityCartesianVector *matrixDsigma;

	long long matrixDsStorageId;
	long long matrixDsStorageVersion;
	long long matrixDualDsStorageId;
	long long matrixDualDsStorageVersion;
	long long matrixDaStorageId;
	long long matrixDaStorageVersion;
	long long matrixDualDaStorageId;
	long long matrixDualDaStorageVersion;
	long long matrixDepsStorageId;
	long long matrixDepsStorageVersion;
	long long matrixDmuStorageId;
	long long matrixDmuStorageVersion;
	long long matrixDsigmaStorageId;
	long long matrixDsigmaStorageVersion;
};



