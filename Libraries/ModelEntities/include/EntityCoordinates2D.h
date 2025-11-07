// @otlicense
// File: EntityCoordinates2D.h
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
#include "EntityBase.h"
#include "OTCore/Point2D.h"

class __declspec(dllexport) EntityCoordinates2D : public EntityBase
{
public:
	EntityCoordinates2D() : EntityCoordinates2D(0, nullptr, nullptr, nullptr) {};
	EntityCoordinates2D(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override;
	virtual entityType getEntityType(void) const override { return DATA; };
	void setCoordinates(const ot::Point2DD& position) { m_location = position; setModified(); };
	const ot::Point2DD& getCoordinates() { return m_location; };
	static std::string className() { return "EntityCoordinates2D"; };
	virtual std::string getClassName(void) const override { return EntityCoordinates2D::className(); };

	virtual std::string serialiseAsJSON() override;
	virtual bool deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept override;


private:
	
	ot::Point2DD m_location;

	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};
