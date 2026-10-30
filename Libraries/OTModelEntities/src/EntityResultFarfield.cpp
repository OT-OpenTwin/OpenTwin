// @otlicense
// File: EntityResultFarfield.cpp
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

// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityResultFarfield.h"
#include "OTModelEntities/Properties/PropertyHelper.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityResultFarfield> registrar("EntityResultFarfield");

EntityResultFarfield::EntityResultFarfield(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityVis2D3D(ID, parent, obs, ms)
{
}

EntityResultFarfield::~EntityResultFarfield()
{
}

bool EntityResultFarfield::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityResultFarfield::storeToDataBase(void)
{
	EntityVis2D3D::storeToDataBase();
}

void EntityResultFarfield::addStorageData(bsoncxx::builder::basic::document &storage)
{
	EntityVis2D3D::addStorageData(storage);

	const std::string serialisedDLA = m_dataLakeAccessCfg.toJson();

	storage.append(
		bsoncxx::builder::basic::kvp("complex", complex),
		bsoncxx::builder::basic::kvp("DataLakeAccessCfg", serialisedDLA)
	);
}

void EntityResultFarfield::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityVis2D3D::readSpecificDataFromDataBase(doc_view, entityMap);

	// Here we can load any special information about the entity
	complex = doc_view["complex"].get_bool();

	const std::string serialisedDLA = doc_view["DataLakeAccessCfg"].get_string().value.data();
	if (!serialisedDLA.empty())
	{
		ot::JsonDocument doc;
		doc.fromJson(serialisedDLA);
		m_dataLakeAccessCfg.setFromJsonObject(doc.getConstObject());
	}

	// During project open this may throw because the model state is not entirely loaded yet. In this case the m_dataLakeAccessCfg should still hold a valid state.
	if (getObserver() != nullptr && getObserver()->getModelServiceState().getProjectIsOpen())
	{
		try
		{
			const std::string projectName = m_queryProperties.getSelectedProject(this);
			std::string collectionName;
			auto associatedCampaign = getObserver()->getMetadataCampaign(projectName, collectionName);
			DataLakeQueryCfg queryCfg;
			queryCfg.setCollectionName(collectionName);
			queryCfg.setSeriesLabel(m_queryProperties.getSelectedSeries(this));
			queryCfg.setValueDescriptionParameters(m_queryProperties.getParameterQueries(this));
			queryCfg.setValueDescriptionSeriesMD(m_queryProperties.getMetadataQueries(this));
			queryCfg.setValueDescriptionQuantities(m_queryProperties.getQuantityQuery(this));

			ot::DataLakeAccessCfg temp = getObserver()->createDataLakeAccessConfig(associatedCampaign.value(), collectionName, queryCfg);
			if (temp.getQueriesByCollection() != m_dataLakeAccessCfg.getQueriesByCollection())
			{
				m_dataLakeAccessCfg = temp;
				setModified();
			}
		}
		catch (std::exception& _e)
		{
			OT_LOG_E(_e.what());
		}
	}

	resetModified();
}

void EntityResultFarfield::createProperties(void)
{
	EntityPropertiesSelection::createProperty("General", "Frequency", frequencies, frequencies.empty() ? "" : frequencies.front(), "", getProperties());
	EntityPropertiesSelection::createProperty("General", "Type", {"Magnitude", "Phase", "Real", "Imaginary"}, "Magnitude", "", getProperties());

	EntityPropertiesDouble::createProperty("Visualization", "Scale factor", 1.0, "", getProperties());
	EntityPropertiesBoolean::createProperty("Visualization", "Show edges", true, "", getProperties());

	EntityPropertiesDouble::createProperty("Phi", "Phi min (deg.)", phiMin, "", getProperties())->setReadOnly(true);
	EntityPropertiesDouble::createProperty("Phi", "Phi max (deg.)", phiMax, "", getProperties())->setReadOnly(true);
	EntityPropertiesInteger::createProperty("Phi", "Phi steps", phiSteps, "", getProperties())->setReadOnly(true);

	EntityPropertiesDouble::createProperty("Theta", "Theta min (deg.)", thetaMin, "", getProperties())->setReadOnly(true);
	EntityPropertiesDouble::createProperty("Theta", "Theta max (deg.)", thetaMax, "", getProperties())->setReadOnly(true);
	EntityPropertiesInteger::createProperty("Theta", "Theta steps", thetaSteps, "", getProperties())->setReadOnly(true);

	EntityPropertiesDouble::createProperty("Internal", "Structure center x", structureCx, "", getProperties())->setVisible(false);
	EntityPropertiesDouble::createProperty("Internal", "Structure center y", structureCy, "", getProperties())->setVisible(false);
	EntityPropertiesDouble::createProperty("Internal", "Structure center z", structureCz, "", getProperties())->setVisible(false);
	EntityPropertiesDouble::createProperty("Internal", "Structure diagonal", structureD, "", getProperties())->setVisible(false);

	m_queryProperties.setProperties(this);

	getProperties().forceResetUpdateForAllProperties();

	updatePropertyVisibilities();
}

bool EntityResultFarfield::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;

	EntityPropertiesSelection* typeProperty = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Type"));
	if (typeProperty != nullptr)
	{
		if (typeProperty->getVisible() != complex)
		{
			typeProperty->setVisible(complex);
			updatePropertiesGrid = true;
		}
	}

	updatePropertiesGrid |= m_queryProperties.updatePropertyVisibility(this);

	return updatePropertiesGrid;
}

bool EntityResultFarfield::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	bool refresh = this->updatePropertyVisibilities();

	if (getObserver() != nullptr)
	{
		try
		{
			const std::string projectName = m_queryProperties.getSelectedProject(this);
			std::string collectionName;
			auto associatedCampaign = getObserver()->getMetadataCampaign(projectName, collectionName);

			assert(associatedCampaign.has_value()); //Only not the case, if the observer has no implementation of the getter.
			refresh = m_queryProperties.updateOptions(this, associatedCampaign.value());

			DataLakeQueryCfg queryCfg;
			queryCfg.setCollectionName(collectionName);
			queryCfg.setSeriesLabel(m_queryProperties.getSelectedSeries(this));
			queryCfg.setValueDescriptionParameters(m_queryProperties.getParameterQueries(this));
			queryCfg.setValueDescriptionSeriesMD(m_queryProperties.getMetadataQueries(this));
			queryCfg.setValueDescriptionQuantities(m_queryProperties.getQuantityQuery(this));

			bool dataRefreshNeeded = queryCfg != m_queryCfg;
			if (dataRefreshNeeded)
			{
				m_dataLakeAccessCfg = getObserver()->createDataLakeAccessConfig(associatedCampaign.value(), collectionName, queryCfg);
			}
			m_queryCfg = queryCfg;
			refresh |= dataRefreshNeeded;
		}
		catch (std::exception& _e)
		{
			std::string message = "Failed to use user given comparisons for data lake access: " + std::string(_e.what());
			OT_LOG_E(message);
		}
	}

	getProperties().forceResetUpdateForAllProperties();

	return refresh; // Notify, whether property grid update is necessary
}

void EntityResultFarfield::setStaticQueryOptions(const ot::DataLakeAccessCfg &_accessCfg, const DataLakeQueryCfg& _queryCfg)
{
	m_dataLakeAccessCfg = _accessCfg;

	m_queryProperties.reset(this, _queryCfg);
	m_queryProperties.lockAllProperties(this);
}
