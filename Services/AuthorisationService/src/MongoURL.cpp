#include "MongoURL.h"

#include <Windows.h>

#include <filesystem>

std::string getMongoURL(std::string databaseURL, std::string dbUsername, std::string dbPassword)
{
	static std::string mongoURL;

	//if (!mongoURL.empty()) return mongoURL;  Deactivate caching, since the connection needs to be checked with different accounts.

	if (databaseURL.substr(0, 4) == "tls@")
	{
		std::string ca_cert_file;

		// Get the path of the executable
#ifdef _WIN32
		char path[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, path, MAX_PATH);
		ca_cert_file = path;
#else
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		ca_cert_flile = std::string(result, (count > 0) ? count : 0);
#endif
		ca_cert_file = ca_cert_file.substr(0, ca_cert_file.rfind('\\'));
		ca_cert_file += "\\Certificates\\ca.pem";

		// Chyeck whether local cert file ca.pem exists
		if (!std::filesystem::exists(ca_cert_file))
		{
			// Get the development root environment variable and build the path to the deployment cert file
			char buffer[4096];
			size_t environmentVariableValueStringLength;

			getenv_s(&environmentVariableValueStringLength, buffer, sizeof(buffer) - 1, "SIM_PLAT_ROOT");

			std::string dev_root = std::string(buffer);
			ca_cert_file = dev_root + "\\Deployment\\Certificates\\ca.pem";
		}

		databaseURL = databaseURL.substr(4) + "/?tls=true&tlsCAFile=" + ca_cert_file;
		//databaseURL = databaseURL.substr(4) + "/?tls=true&tlsAllowInvalidCertificates=false&tlsCAFile=C:/ca.pem";
		//databaseURL = databaseURL.substr(4) + "/?tls=true&tlsCAFile=C:/ca.pem";
		//databaseURL = databaseURL.substr(4) + "/?tls=true";
	}

	std::string uriStr = "mongodb://" + dbUsername + ":" + dbPassword + "@" + databaseURL;

	mongoURL = uriStr;
	return uriStr;
}
