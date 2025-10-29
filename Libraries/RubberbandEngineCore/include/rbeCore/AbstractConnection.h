// @otlicense

#pragma once

// RBE header
#include <rbeCore/dataTypes.h>

// C++ header
#include <string>
#include <sstream>

namespace rbeCore {

	class RubberbandEngine;

	class RBE_API_EXPORT AbstractConnection {
	public:

		enum connectionType {
			ctLine,
			ctCircle,
			ctHistory
		};

		AbstractConnection(connectionType _type);
		virtual ~AbstractConnection() {}

		// ######################################################################################

		// Getter

		connectionType type(void) const { return m_type; }

		bool ignoreInHistory(void) const { return m_ignoreInHistory; }

		// ######################################################################################

		// Setter

		void setIgnoreInHistory(bool _ignore) { m_ignoreInHistory = _ignore; }

		// ######################################################################################

		// Abstract functions

		virtual std::string debugInformation(const std::string& _prefix) = 0;

		virtual void addToJsonArray(RubberbandEngine * _engine, std::stringstream& _array, bool& _first) = 0;

	private:
		connectionType		m_type;
		bool				m_ignoreInHistory;

		AbstractConnection() = delete;
		AbstractConnection(AbstractConnection&) = delete;
		AbstractConnection& operator = (AbstractConnection&) = delete;
	};

}