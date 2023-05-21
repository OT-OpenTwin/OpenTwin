//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTsci/OTsciAPIExport.h"

#pragma warning(disable: 4251)

#include <string>
#include <list>

namespace aci {

	class ExternalDllScript;
	class InterpreterCore;
	
	class OTSCI_API_EXPORT ScriptLoader {
	public:
		ScriptLoader(InterpreterCore * _core);
		virtual ~ScriptLoader();

		void loadDll(const std::wstring& _path, const std::wstring& _filename);
		void loadDllsFromDirectory(const std::wstring& _directoryPath);

		void unloadScripts(void);

	private:
		
		std::list<ExternalDllScript *>	m_externalDlls;
		InterpreterCore *				m_core;

		ScriptLoader() = delete;
		ScriptLoader(ScriptLoader&) = delete;
		ScriptLoader& operator = (ScriptLoader&) = delete;
	};

}
