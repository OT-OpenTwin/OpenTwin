//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include <Windows.h>
#include <string>

namespace aci {

	class InterpreterObject;

	class ExternalDllScript {
	public:
		typedef InterpreterObject ** (__stdcall *DLLGenerateObjectsFunctionType)(int& _count);

		ExternalDllScript(HINSTANCE _libraryInstance, DLLGenerateObjectsFunctionType _functionRef, InterpreterObject* _interpreterObject, const std::wstring& _filePath)
			: m_libraryInstance(_libraryInstance), m_functionRef(_functionRef), m_interptreterObject(_interpreterObject), m_filePath(_filePath) {};
		virtual ~ExternalDllScript() {};

		HINSTANCE libraryInstance(void) { return m_libraryInstance; }
		DLLGenerateObjectsFunctionType generateFunctionRef(void) { return m_functionRef; }
		InterpreterObject * interptreterObject(void) { return m_interptreterObject; }
		const std::wstring& filePath(void) const { return m_filePath; }		

	private:
		HINSTANCE							m_libraryInstance;
		DLLGenerateObjectsFunctionType		m_functionRef;
		InterpreterObject *					m_interptreterObject;
		std::wstring						m_filePath;

		ExternalDllScript() = delete;
		ExternalDllScript(ExternalDllScript&) = delete;
		ExternalDllScript& operator = (ExternalDllScript&) = delete;
	};
}
