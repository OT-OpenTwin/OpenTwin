#pragma once

#include "OpenTwinSystem/SystemAPIExport.h"
#include "OpenTwinSystem/SystemTypes.h"

#include <string>
#include <map>
#include <list>
#include <mutex>

#pragma warning (disable:4251)

namespace ot {

	class OT_SYS_API_EXPORT PortRange {
	public:
		PortRange();
		PortRange(ot::port_t _from, ot::port_t _to);
		PortRange(const PortRange& _other);
		
		PortRange& operator = (const PortRange& _other);

		//! @brief Will set the port range
		//! Note that the maximum port number must not exceed 49,151
		void set(ot::port_t _from, ot::port_t _to);

		//! Note that the maximum port number must not exceed 49,151
		void setFrom(ot::port_t _from) { m_from = _from; };

		//! Note that the maximum port number must not exceed 49,151
		void setTo(ot::port_t _to) { m_to = _to; };

		ot::port_t from(void) const { return m_from; };
		ot::port_t to(void) const { return m_to; };

	private:
		ot::port_t m_from;
		ot::port_t m_to;
	};

	class OT_SYS_API_EXPORT PortManager {
	public:		
		static PortManager& instance(void);

		//! Note that the maximum port number must not exceed 49,151
		void addPortRange(const PortRange& _range);

		//! Note that the maximum port number must not exceed 49,151
		void addPortRange(ot::port_t _from, ot::port_t _to);

		void setPortNotInUse(ot::port_t _portNumber);
		bool isPortInUse(ot::port_t _portNumber);

		ot::port_t determineAndBlockAvailablePort(void);

	private:
		std::list<PortRange>			m_ranges;
		std::map<ot::port_t, bool>		m_portsInUse;

		PortManager();
		virtual ~PortManager();
	};

}