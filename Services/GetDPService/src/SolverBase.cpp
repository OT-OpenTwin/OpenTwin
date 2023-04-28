#include "SolverBase.h"

#include <windows.h> // winapi
#include <UserEnv.h>
#include <codecvt>
#include <stdlib.h>

void SolverBase::setData(EntityBase* _solverEntity, const std::string& _meshDataName, std::map<ot::UID, ot::EntityInformation>& _meshItemInfo,
						 std::map<ot::UID, EntityProperties>& _entityProperties, std::map<std::string, size_t>& _groupNameToIdMap,
						 std::map<std::string, EntityProperties>& _materialProperties)
{
	solverEntity = _solverEntity;
	meshDataName = _meshDataName;
	meshItemInfo = _meshItemInfo;
	entityProperties = _entityProperties;
	groupNameToIdMap = _groupNameToIdMap;
	materialProperties = _materialProperties;
}

bool SolverBase::isPECMaterial(const std::string& materialName)
{
	EntityPropertiesSelection* materialType = dynamic_cast<EntityPropertiesSelection*>(materialProperties[materialName].getProperty("Material type"));
	
	if (materialType != nullptr)
	{
		return (materialType->getValue() == "PEC");
	}

	return false;
}

void SolverBase::runSolverExe(const std::string& inputFileName, const std::string& solvTarget, const std::string& postTarget, const std::string& workingDirectory)
{
	std::string exePath = readEnvironmentVariable("OPENTWIN_DEV_ROOT");
	if (exePath.empty())
	{
		// Get the path of the executable
#ifdef _WIN32
		char path[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, path, MAX_PATH);
		exePath = path;
#else
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		ca_cert_flile = std::string(result, (count > 0) ? count : 0);
#endif
		exePath = exePath.substr(0, exePath.rfind('\\'));
	}
	else
	{
		exePath += "\\Deployment";
	}

	std::string commandLine = "\"" + exePath + "\\getdp.exe\"" + " model -solve " + solvTarget + " -post " + postTarget;

	if (!runExecutableAndWaitForCompletion(commandLine, workingDirectory))
	{
		throw std::string("ERROR: Unable to run GETDP executable. Command line: " + commandLine);
	}
}

#pragma comment(lib, "userenv.lib")

bool SolverBase::runExecutableAndWaitForCompletion(std::string commandLine, std::string workingDirectory)
{
	HANDLE hToken = NULL;
	BOOL ok = OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &hToken);
	assert(ok);

	wchar_t* penv = nullptr;
	ok = CreateEnvironmentBlock((void**)&penv, hToken, TRUE);
	assert(ok);

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	std::wstring wideString = strconverter.from_bytes(commandLine);

	LPWSTR cl = new WCHAR[wideString.size() + 1];
	wcscpy_s(cl, wideString.size() + 1, wideString.c_str());

	std::wstring workDir = strconverter.from_bytes(workingDirectory);
	LPWSTR wd = new WCHAR[workDir.size() + 1];
	wcscpy_s(wd, workDir.size() + 1, workDir.c_str());

	STARTUPINFO info;
	PROCESS_INFORMATION processInfo;

	ZeroMemory(&info, sizeof(info));
	info.cb = sizeof(info);
	ZeroMemory(&processInfo, sizeof(processInfo));

	bool success = true;

	if (CreateProcess(NULL, cl, NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT, penv, wd, &info, &processInfo))
	{
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}
	else
	{
		success = false;
	}

	delete[] cl;
	cl = nullptr;

	DestroyEnvironmentBlock(penv);

	return success;
}

std::string SolverBase::readEnvironmentVariable(const std::string& variableName)
{
	std::string variableValue;

	const int nSize = 32767;
	char* buffer = new char[nSize];

	if (GetEnvironmentVariableA(variableName.c_str(), buffer, nSize))
	{
		variableValue = buffer;
	}

	return variableValue;
}
