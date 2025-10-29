// @otlicense

#include "SolverBase.h"

#include "OTServiceFoundation/UiComponent.h"

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

void SolverBase::runSolverExe(const std::string& inputFileName, const std::string& solvTarget, const std::string& postTarget, const std::string& workingDirectory, ot::components::UiComponent* uiComponent)
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

	if (!runExecutableAndWaitForCompletion(commandLine, workingDirectory, uiComponent))
	{
		throw std::string("ERROR: Unable to run GETDP executable. Command line: " + commandLine);
	}
}

#pragma comment(lib, "userenv.lib")

bool SolverBase::runExecutableAndWaitForCompletion(std::string commandLine, std::string workingDirectory, ot::components::UiComponent* uiComponent)
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

	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;

	SECURITY_ATTRIBUTES saAttr;

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	bool success = true;

	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
	{
		success = false;
	}
	else
	{
		if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		{
			CloseHandle(g_hChildStd_OUT_Wr);
			success = false;
		}
		else
		{
			ZeroMemory(&info, sizeof(info));
			info.cb = sizeof(info);
			info.hStdError = g_hChildStd_OUT_Wr;
			info.hStdOutput = g_hChildStd_OUT_Wr;
			info.dwFlags |= (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
			info.wShowWindow = SW_HIDE;

			ZeroMemory(&processInfo, sizeof(processInfo));

			if (CreateProcess(NULL, cl, NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT | ABOVE_NORMAL_PRIORITY_CLASS, penv, wd, &info, &processInfo))
			{
				CloseHandle(g_hChildStd_OUT_Wr);
				ReadFromPipe(g_hChildStd_OUT_Rd, uiComponent);

				WaitForSingleObject(processInfo.hProcess, INFINITE);
				CloseHandle(processInfo.hProcess);
				CloseHandle(processInfo.hThread);
			}
			else
			{
				CloseHandle(g_hChildStd_OUT_Wr);
				success = false;
			}
		}
	}

	delete[] cl;
	cl = nullptr;

	DestroyEnvironmentBlock(penv);

	return success;
}

void SolverBase::ReadFromPipe(HANDLE g_hChildStd_OUT_Rd, ot::components::UiComponent* uiComponent)

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
{
	#define BUFSIZE 4096 

	DWORD dwRead;
	CHAR chBuf[BUFSIZE];
	BOOL bSuccess = FALSE;

	for (;;)
	{
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) break;

		std::string text(chBuf, dwRead);

		solverOutput << text;

		text.erase(text.find_last_not_of(" \t\n\r\f\v") + 1);
		uiComponent->displayMessage(text);

		if (!bSuccess) break;
	}
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
