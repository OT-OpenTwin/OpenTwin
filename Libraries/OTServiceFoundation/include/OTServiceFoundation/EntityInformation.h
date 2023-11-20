#pragma once

// OpenTwin header
#include "OTServiceFoundation/FoundationAPIExport.h"

// std header
#include <string>
#include <Types.h>

namespace ot {

	class OT_SERVICEFOUNDATION_API_EXPORT EntityInformation
	{
	public:
		EntityInformation() : m_id(0), m_version(0) {};
		virtual ~EntityInformation() {};

		void setID(UID _id) { m_id = _id; }
		void setVersion(UID _version) { m_version = _version; }
		void setName(std::string _name) { m_name = _name; }
		void setType(std::string _type) { m_type = _type; }

		UID getID(void) const { return m_id; }
		UID getVersion(void) const { return m_version; }
		std::string getName(void) const { return m_name; }
		std::string getType(void) const { return m_type; }

	private:
		UID				m_id;
		UID				m_version;
		std::string		m_name;
		std::string		m_type;
	};

}