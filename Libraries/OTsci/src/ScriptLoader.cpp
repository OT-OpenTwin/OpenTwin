//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include <aci/ScriptLoader.h>
#include <aci/InterpreterCore.h>
#include <aci/InterpreterObject.h>
#include <aci/AbstractPrinter.h>
#include <aci/ExternalDllScript.h>
#include <aci/aDir.h>
#include <aci/aFile.h>

aci::ScriptLoader::ScriptLoader(InterpreterCore * _core) : m_core(_core) {}

aci::ScriptLoader::~ScriptLoader() {

}

void aci::ScriptLoader::loadDll(const std::wstring& _path, const std::wstring& _filename) {
	m_core->printer()->setColor(255, 255, 255);
	m_core->printer()->print(L"[script loader] Load library: " + _filename);
	HINSTANCE hGetProcIDDLL = LoadLibrary(_path.c_str());
	
	if (hGetProcIDDLL) {
		// Resolve function address here
		ExternalDllScript::DLLGenerateObjectsFunctionType func = (ExternalDllScript::DLLGenerateObjectsFunctionType)GetProcAddress(hGetProcIDDLL, "generateObjects");
		if (func) {
			int objCt{ 0 };
			aci::InterpreterObject ** obj = func(objCt);
			int ct{ 0 };
			
			for (int o{ 0 }; o < objCt; o++) {
				if (obj[o]) {
					ExternalDllScript * newScript = new ExternalDllScript(hGetProcIDDLL, func, obj[o], _path);
					m_core->addScriptObject(obj[o]);
					ct++;
					m_externalDlls.push_back(newScript);
				}
			}

			if (ct == 0) {
				m_core->printer()->setColor(255, 150, 50);
				m_core->printer()->print(L"  DONE");
				m_core->printer()->setColor(255, 255, 255);
				m_core->printer()->print(L" (no commands loaded)\n");
			}
			else {
				m_core->printer()->setColor(0, 255, 0);
				m_core->printer()->print(L"  SUCCESS");
				m_core->printer()->setColor(255, 255, 255);
				m_core->printer()->print(" (" + std::to_string(ct) + " commands loaded)\n");
			}
		}
		else {
			m_core->printer()->setColor(255, 0, 0);
			m_core->printer()->print(L"  FAILED (Entry point not found)\n");
		}
	}
	else {
		m_core->printer()->setColor(255, 0, 0);
		m_core->printer()->print(L"  FAILED (dll not loaded. Error code: " + std::to_wstring(GetLastError()) + L")\n");
	}
}

void aci::ScriptLoader::loadDllsFromDirectory(const std::wstring& _directoryPath) {
	m_core->printer()->print(L"[script loader] Searching for files at: " + _directoryPath + L"\n");
	aDir directory(L"", _directoryPath);
	directory.scanFiles(false);
	m_core->printer()->print(L"[script loader] Filter for *.dll files\n");
	directory.filterFilesWithWhitelist({ L".dll" });

	for (auto file : directory.files()) {
		loadDll(file->fullPath(), file->name());
	}
}

void aci::ScriptLoader::unloadScripts(void) {
	for (auto script : m_externalDlls) {
		m_core->removeScriptObject(script->interptreterObject()->key(), true);
		FreeLibrary(script->libraryInstance());
		delete script;
	}
	m_externalDlls.clear();
}
