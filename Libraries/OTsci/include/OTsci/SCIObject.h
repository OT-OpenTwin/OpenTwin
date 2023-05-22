//! @file SCIObject.h
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTsci/OTsciAPIExport.h"
#include "OpenTwinCore/Color.h"

#include <vector>
#include <list>
#include <string>

namespace ot {
	

	class OTSCI_API_EXPORT SCIObject {
	public:

		//! @brief Result when the handle function of an SCI object was called
		enum SCIHandleResult {
			Ok                 = 0, //! @brief Handle was ok
			RequestNextCommand = 1, //! @brief Handle was ok, request next command
			Failed             = 2, //! @brief Handle failed
			UnknownCommand     = 3  //! @brief Unknown command
			
		};

		SCIObject();
		virtual ~SCIObject() {};

		//! @brief Return the key to this object
		virtual std::wstring key(void) const = 0;

		//! @brief Handle to provided command
		virtual SCIHandleResult handle(const std::wstring& _command, const std::vector<std::wstring>& _params) = 0;

		// ################################################################################################################################

		// Output

		void showInfo(void);

		void printDelimiterLine(void);

		// ################################################################################################################################

		// File operations

		bool readDatSCIFile(std::wstring& _data, const std::wstring& _filename, bool _showLog);
		bool readDatSCIFile(std::list<std::wstring>& _data, const std::wstring& _filename, bool _showLog);

		bool writeDatSCIFile(const std::wstring& _data, const std::wstring& _filename, bool _showLog);
		bool writeDatSCIFile(const std::list<std::wstring>& _data, const std::wstring& _filename, bool _showLog);

		// ################################################################################################################################

		// Setter

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
		std::list<std::wstring> filesInDatSCIDirectory(const std::wstring& _subdirectory = std::wstring());

		std::list<std::wstring> subdirectories(const std::wstring& _directoryPath);
		std::list<std::wstring> subdirectoriesInDatSCIDirectory(const std::wstring& _subdirectory = std::wstring());

		// ################################################################################################################################

		// Static functions

		static std::wstring isolateFilename(const std::wstring& _path);
		
	protected:
		virtual void showCommandInfo(void) = 0;
	};
}