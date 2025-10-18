//! @file NewModelStateInfo.h
//! @authors Jan Wagner, Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTModelAPI/OTModelAPIExport.h"
#include "EntityBase.h"

// std header
#include <list>

#pragma warning(disable: 4251)

namespace ot {

	//! @brief The NewModelStateInfo class is used to collect information about new entities that should be added to the model.
	class OT_MODELAPI_API_EXPORT NewModelStateInfo {
		OT_DECL_DEFCOPY(NewModelStateInfo)
		OT_DECL_DEFMOVE(NewModelStateInfo)
	public:
		NewModelStateInfo() = default;
		~NewModelStateInfo() = default;

		//! @brief Adds a topology entity to the end of the list of topology entities.
		//! @param _entityID Topology entity ID.
		//! @param _entityVersion Topology entity version.
		//! @param _forceVisible If set to true, the topology entity will be forced to be visible in the visualization.
		void addTopologyEntity(ot::UID _entityID, ot::UID _entityVersion, bool _forceVisible = false);

		//! @brief Adds a topology entity to the end of the list of topology entities.
		//! @param _entityTopology Topology entity information.
		//! @param _forceVisible If set to true, the topology entity will be forced to be visible in the visualization.
		void addTopologyEntity(const EntityBase& _entityTopology, bool _forceVisible = false);

		//! @brief Adds a data entity to the end of the list of data entities.
		//! @param _parentTopoEntityID Parent topology entity ID.
		//! @param _entityID Data entity ID.
		//! @param _entityVersion Data entity version.
		void addDataEntity(ot::UID _parentTopoEntityID, ot::UID _entityID, ot::UID _entityVersion);

		//! @brief Adds a data entity to the end of the list of data entities.
		//! @param _parentTopoEntityID Parent topology entity ID.
		//! @param _entityData Data entity information.
		void addDataEntity(ot::UID _parentTopoEntityID, const EntityBase& _entityData);

		//! @brief Adds a data entity to the end of the list of data entities.
		//! @param _parentTopoEntity Parent topology entity information.
		//! @param _entityData Data entity information.
		void addDataEntity(const EntityBase& _parentTopoEntity, const EntityBase& _entityData);

		//! @brief Adds a data entity to the end of the list of data entities.
		//! @param _parentTopoEntity Parent topology entity information.
		//! @param _entityID Data entity ID.
		//! @param _entityVersion Data entity version.
		void addDataEntity(const EntityBase& _parentTopoEntity, ot::UID _entityID, ot::UID _entityVersion);

		//! @brief Returns true if there are any entities stored in this object.
		bool hasEntities() const { return hasTopologyEntities() || hasDataEntities(); };

		//! @brief Returns true if there are any topology entities stored in this object.
		bool hasTopologyEntities() const { return !m_topologyEntityIDs.empty(); };

		//! @brief Returns the lists of topology entity IDs.
		const std::list<ot::UID>& getTopologyEntityIDs() const { return m_topologyEntityIDs; };

		//! @brief Returns the lists of topology entity versions.
		const std::list<ot::UID>& getTopologyEntityVersions() const { return m_topologyEntityVersions; };

		//! @brief Returns the lists of topology entity force visibility flags.
		const std::list<bool>& getTopologyForceVisible() const { return m_topologyForceVisible; };

		//! @brief Returns true if there are any data entities stored in this object.
		bool hasDataEntities() const { return !m_dataEntityIDs.empty(); };

		//! @brief Returns the lists of data entity IDs.
		const std::list<ot::UID>& getDataEntityIDs() const { return m_dataEntityIDs; };

		//! @brief Returns the lists of data entity versions.
		const std::list<ot::UID>& getDataEntityVersions() const { return m_dataEntityVersions; };

		//! @brief Returns the lists of data entity parent topology entity IDs.
		const std::list<ot::UID>& getDataEntityParentIDs() const { return m_dataEntityParentIDs; };

	private:
		std::list<ot::UID> m_topologyEntityIDs;
		std::list<ot::UID> m_topologyEntityVersions;
		std::list<bool>    m_topologyForceVisible;

		std::list<ot::UID> m_dataEntityIDs;
		std::list<ot::UID> m_dataEntityVersions;
		std::list<ot::UID> m_dataEntityParentIDs;
	};
}


