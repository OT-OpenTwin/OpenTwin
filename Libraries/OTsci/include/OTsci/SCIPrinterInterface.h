//! @file SCIPrinterInterface.h
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTsci/OTsciAPIExport.h"
#include "OpenTwinCore/Color.h"

// std header
#include <string>

namespace ot {

	class OTSCI_API_EXPORT SCIPrinterInterface {
	public:
		SCIPrinterInterface() {};
		virtual ~SCIPrinterInterface() {};
		
		virtual void print(const std::string& _str) = 0;
		virtual void print(const std::wstring& _str) = 0;
		virtual void setColor(const ot::Color& _color) = 0;

		void print(const char* _str) { print(std::string(_str)); };
		void print(const wchar_t* _str) { print(std::wstring(_str)); };
		void setColor(int _r, int _g, int _b, int _a = 255) { setColor(ot::Color(_r, _g, _b, _a)); };

	protected:
		SCIPrinterInterface(const SCIPrinterInterface&) = delete;
		SCIPrinterInterface& operator = (const SCIPrinterInterface&) = delete;
	};

}