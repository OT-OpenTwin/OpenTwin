//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTsci/OTsciAPIExport.h"
#include <aci/InterpreterObject.h>
#include <aci/Color.h>

#include <vector>
#include <list>
#include <string>

namespace aci {
	class AbstractPrinter;
	class InterpreterCore;
	class AbstractInterpreterNotifier;

	class OTSCI_API_EXPORT InterpreterObject {
	public:
		InterpreterObject();
		virtual ~InterpreterObject() {}

		virtual std::wstring key(void) const = 0;

		virtual bool handle(const std::wstring& _command, const std::vector<std::wstring>& _params) = 0;

		// ################################################################################################################################

		// Output

		void showInfo(void);

		void printDelimiterLine(void);

		// ################################################################################################################################

		// File operations

		bool readDataFile(std::wstring& _data, const std::wstring& _filename, bool _showLog);
		bool readDataFile(std::list<std::wstring>& _data, const std::wstring& _filename, bool _showLog);

		bool writeDataFile(const std::wstring& _data, const std::wstring& _filename, bool _showLog);
		bool writeDataFile(const std::list<std::wstring>& _data, const std::wstring& _filename, bool _showLog);

		// ################################################################################################################################

		// Setter

		void attachPrinter(AbstractPrinter * _printer) { m_printer = _printer; }
		void attachCore(InterpreterCore * _core) { m_core = _core; }
		void attachNotifier(AbstractInterpreterNotifier * _notifier) { m_notifier = _notifier; }

		void disableInput(void);
		void enableInput(void);

		void print(const char * _message);
		void print(const wchar_t * _message);
		void print(bool _value);
		void print(const std::string& _message);
		void print(const std::wstring& _message);

		void setColor(int _r, int _g, int _b, int _a = 255);
		void setColor(const Color& _color);
		void setColor(Color::DefaultColor _color);

		// ################################################################################################################################

		// Getter

		std::list<std::wstring> filesInDirectory(const std::wstring& _directoryPath);
		std::list<std::wstring> filesInDataDirectory(const std::wstring& _subdirectory = std::wstring());

		std::list<std::wstring> subdirectories(const std::wstring& _directoryPath);
		std::list<std::wstring> subdirectoriesInDataDirectory(const std::wstring& _subdirectory = std::wstring());

		// ################################################################################################################################

		// Static functions

		static std::wstring isolateFilename(const std::wstring& _path);
		
	protected:
		virtual void showCommandInfo(void) = 0;

	private:
		InterpreterCore *				m_core;
		AbstractPrinter *				m_printer;
		AbstractInterpreterNotifier *	m_notifier;
	};
}