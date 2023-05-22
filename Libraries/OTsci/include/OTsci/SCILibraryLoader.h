//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTsci/OTsciAPIExport.h"
#include "OpenTwinSystem/SystemTypes.h"

#pragma warning(disable: 4251)

// std header
#include <string>
#include <list>

namespace ot {

	class ExternalSCILibraryHandler;
	class SCIObject;
	
	class OTSCI_API_EXPORT ScriptLoader {
	public:
		ScriptLoader() {};
		virtual ~ScriptLoader() {};

		void loadDll(const std::wstring& _path, const std::wstring& _filename);
		void loadDllsFromDirectory(const std::wstring& _directoryPath);

		void unloadScripts(void);

	private:
		std::list<ExternalSCILibraryHandler*>	m_externalDlls;

		ScriptLoader() = delete;
		ScriptLoader(ScriptLoader&) = delete;
		ScriptLoader& operator = (ScriptLoader&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @brief Describes a external sci library that was attached to the library loader
	class __declspec(dllexport) ExternalSCILibraryHandler {
	public:
		typedef SCIObject** (__stdcall* GenerateObjectsFunctionRef)(int& _count);

		ExternalSCILibraryHandler(ExternalLibraryInstance_t _libraryInstance, GenerateObjectsFunctionRef _functionRef, SCIObject* _object, const std::wstring& _filePath)
			: m_libraryInstance(_libraryInstance), m_functionRef(_functionRef), m_object(_object), m_filePath(_filePath) {};
		virtual ~ExternalSCILibraryHandler() {};

		void setLibraryInstance(ExternalLibraryInstance_t _instance) { m_libraryInstance = _instance; };
		ExternalLibraryInstance_t libraryInstance(void) { return m_libraryInstance; };

		void setGenerateObjectsFunctionRef(GenerateObjectsFunctionRef _referefnce) { m_functionRef = _referefnce; };
		GenerateObjectsFunctionRef generateObjectsFunctionRef(void) { return m_functionRef; };

		void setObject(SCIObject* _object) { m_object = _object; };
		SCIObject* getObject(void) { return m_object; };

		void setFilePath(const std::wstring& _path) { m_filePath = _path; };
		const std::wstring& filePath(void) const { return m_filePath; };

	private:
		ExternalLibraryInstance_t  m_libraryInstance;
		GenerateObjectsFunctionRef m_functionRef;
		SCIObject*                 m_object;
		std::wstring               m_filePath;

		ExternalSCILibraryHandler() = delete;
		ExternalSCILibraryHandler(ExternalSCILibraryHandler&) = delete;
		ExternalSCILibraryHandler& operator = (ExternalSCILibraryHandler&) = delete;
	};
}
