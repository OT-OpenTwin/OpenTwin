//! @file PortManager.h
//! @author Alexander Kuester (alexk95)
//! @date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/SystemTypes.h"
#include "OTSystem/SystemAPIExport.h"

// std header
#include <list>
#include <unordered_set>

#pragma warning (disable:4251)

namespace ot {

	//! @class PortManager
	//! @brief The PortManager is used to store and check blocked ports.
	class OT_SYS_API_EXPORT PortManager {
	public:
		PortManager() {};
		PortManager(ot::port_t _startingPort, ot::port_t _maxPort = static_cast<ot::port_t>(ot::maxPortNumber));
		PortManager(const PortManager&) = default;
		PortManager(PortManager&&) = default;
		~PortManager() {};

		PortManager& operator=(const PortManager&) = default;
		PortManager& operator=(PortManager&&) = default;

		//! @brief Adds the port range to be used to determine available ports.
		//! @param _from Starting port (min port).
		//! @param _to Final port (max port).
		void addPortRange(ot::port_t _from, ot::port_t _to);

		//! @brief Set the specified port as "not in use".
		//! @param _portNumber Port to unset.
		void freePort(ot::port_t _portNumber);

		//! @brief Returns true if the provided port is occupied.
		bool isPortInUse(ot::port_t _portNumber) const;

		//! @brief Determines the next available port and blocks it.
		ot::port_t determineAndBlockAvailablePort(void);

		bool hasPortRanges() const { return !m_ranges.empty(); };

	private:
		using PortRange = std::pair<ot::port_t, ot::port_t>;

		std::list<PortRange>           m_ranges;     //! @brief Available ranges.
		std::unordered_set<ot::port_t> m_portsInUse; //! @brief Blocked ports.
	};

}