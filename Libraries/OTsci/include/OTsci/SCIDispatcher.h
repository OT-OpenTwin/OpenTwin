//! @file SCIDispatcher.h
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTsci/OTsciAPIExport.h"

#pragma warning(disable:4251)

#include <map>
#include <vector>
#include <list>
#include <string>

namespace ot {
	class SCILibraryLoader;
	class SCINotifierInterface;
	class SCINotifierInterface;

	class OTSCI_API_EXPORT SCIDispatcher {
	public:
		static SCIDispatcher& instance(void);

		// ########################################################################################

		// Setter

		void setNotifier(ot::SCINotifierInterface* _notifier) { m_notifier = _notifier; }
		void setPrinter(ot::SCIPrinterInterface* _printer);
		void setAutoClean(bool _isAutoClean) { m_autoClean = _isAutoClean; }
		void addScriptObject(ot::InterpreterObject * _obj);
		void removeScriptObject(const std::wstring& _key, bool _deleteObject = true);
		void setCurrentPath(const std::wstring& _path);

		// ########################################################################################

		// Event handler

		bool handle(const std::wstring& _message);
		bool cmdData(const std::vector<std::wstring>& _args);
		void showHelp(void);

		// ########################################################################################

		// Getter

		std::wstring currentPath(void) const;
		ot::AbstractPrinter * printer(void) { return m_printer; }

		// ########################################################################################

		// Static functions

		static void extractClassicSyntax(std::vector<std::wstring>& _dest, const std::wstring& _origin);

		// ########################################################################################

		// Getter

		const std::map<std::wstring, ot::InterpreterObject *>& objects(void) const { return m_objects; }
		std::map<std::wstring, ot::InterpreterObject *>& objects(void) { return m_objects; }

		ot::InterpreterObject * findFirstMatchingItem(const std::wstring& _key);

		SCILibraryLoader* libraryLoader(void) { return m_scriptLoader; }

	private:
		ot::AbstractPrinter *								m_printer;
		ot::AbstractInterpreterNotifier *						m_notifier;
		SCILibraryLoader *									m_scriptLoader;
		bool												m_autoClean;

		std::list<std::wstring>								m_path;

		std::map<std::wstring, ot::InterpreterObject *>	m_objects;

		SCIDispatcher();
		virtual ~SCIDispatcher();
		SCIDispatcher(const SCIDispatcher&) = delete;
		SCIDispatcher& operator = (const SCIDispatcher&) = delete;
	};
}