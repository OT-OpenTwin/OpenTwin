/*
 *	credentials.h
 *
 */

#pragma once
#pragma warning(disable : 4251)

// OpenTwin header
#include "OTServiceFoundation/FoundationAPIExport.h"

// std header
#include <string>

namespace ot {
	class OT_SERVICEFOUNDATION_API_EXPORT UserCredentials
	{
	public: 
		UserCredentials() {};
		virtual ~UserCredentials() {};

		void setUserName(std::string name) { userName = encryptString(name); }
		std::string getUserName(void) { return decryptString(userName); }

		void setPassword(std::string pwd) { userPassword = encryptString(pwd); }
		void setEncryptedPassword(std::string pwd) { userPassword = pwd; }
		std::string getEncryptedPassword(void) { return userPassword; }
		std::string getPassword(void) { return decryptString(userPassword); }

		static std::string encryptString(std::string data);
		static std::string decryptString(std::string data);

	private:
		std::string userName;
		std::string userPassword;
	};
}
