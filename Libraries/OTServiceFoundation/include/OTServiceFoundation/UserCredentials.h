//! @file UserCredentials.h
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once
#pragma warning(disable : 4251)

// OpenTwin header
#include "OTServiceFoundation/FoundationAPIExport.h"

// std header
#include <string>

namespace ot {
	class OT_SERVICEFOUNDATION_API_EXPORT UserCredentials {
	public:
		UserCredentials() {};
		virtual ~UserCredentials() {};

		void setUserName(const std::string& _name) { m_userName = encryptString(_name); };
		std::string getUserName(void) const { return decryptString(m_userName); };

		void setPassword(const std::string& _pwd) { m_userPassword = encryptString(_pwd); };
		void setEncryptedPassword(std::string _pwd) { m_userPassword = _pwd; };
		std::string getEncryptedPassword(void) const { return m_userPassword; };
		std::string getPassword(void) const { return decryptString(m_userPassword); };

		static std::string encryptString(const std::string& _raw);
		static std::string decryptString(const std::string& _encrypted);

	private:
		std::string m_userName;
		std::string m_userPassword;
	};
}
