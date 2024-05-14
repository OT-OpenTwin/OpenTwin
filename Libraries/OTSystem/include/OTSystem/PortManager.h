//! \file PortManager.h
//! \author Alexander Kuester (alexk95)
//! \date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTSystem/SystemAPIExport.h"
#include "OTSystem/SystemTypes.h"

#include <string>
#include <map>
#include <list>
#include <mutex>

#pragma warning (disable:4251)

namespace ot {

	//! \class PortRange
	//! \brief The PortRange may be used to store a Port range.
	//! \note Note that the port number must not exceed 49,151.
	class OT_SYS_API_EXPORT PortRange {
	public:
		//! \brief Default constructor.
		PortRange();

		//! \brief Assignment constructor.
		//! \param _from The starting port.
		//! \param _to The final port.
		PortRange(ot::port_t _from, ot::port_t _to);

		//! \brief Copy constructor.
		//! \param _other The other range.
		PortRange(const PortRange& _other);
		
		//! \brief Assignment operator.
		//! \param _other The other range.
		PortRange& operator = (const PortRange& _other);

		//! \brief Will set the port range.
		void set(ot::port_t _from, ot::port_t _to);

		//! \brief Set the starting port.
		void setFrom(ot::port_t _from) { m_from = _from; };

		//! \brief Set the final port.
		void setTo(ot::port_t _to) { m_to = _to; };

		//! \brief Starting port.
		ot::port_t from(void) const { return m_from; };

		//! \brief Final port.
		ot::port_t to(void) const { return m_to; };

	private:
		ot::port_t m_from; //! \brief Starting port.
		ot::port_t m_to; //! \brief Final port.
	};

	//! \class PortManager
	//! \brief The PortManager may be used to determine available ports and store the information about ports occupied by the PortManager.
	class OT_SYS_API_EXPORT PortManager {
	public:
		//! \brief Returns the global instance.
		static PortManager& instance(void);

		//! \brief Adds the port range to be used to determine available ports.
		//! \param _range Range to add.
		void addPortRange(const PortRange& _range);

		//! \brief Adds the port range to be used to determine available ports.
		//! \note Note that the port number must not exceed 49,151.
		//! \param _from Starting port.
		//! \param _to Final port.
		void addPortRange(ot::port_t _from, ot::port_t _to);

		//! \brief Set the specified port as "not in use".
		//! \param _portNumber Port to unset.
		void setPortNotInUse(ot::port_t _portNumber);

		//! \brief Returns true if the provided port is occupied.
		bool isPortInUse(ot::port_t _portNumber);

		//! \brief Determines the next available port and stores its information.
		//! The PortManager will determine the next available port.
		//! If a port was found, the port number will be saved as a "port in use".
		ot::port_t determineAndBlockAvailablePort(void);

	private:
		std::list<PortRange>			m_ranges; //! \brief Available ranges.
		std::map<ot::port_t, bool>		m_portsInUse; //! \brief Blocked ports.

		//! \brief Private constructor.
		PortManager();

		//! \brief Private destructor.
		virtual ~PortManager();
	};

}