//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTsci/OTsciAPIExport.h"
#include <aci/Color.h>

#include <string>

namespace aci {
	class OTSCI_API_EXPORT AbstractPrinter {
	public:
		AbstractPrinter() {}
		virtual ~AbstractPrinter() {}
		
		virtual void print(const std::string& _str) = 0;
		virtual void print(const std::wstring& _str) = 0;
		virtual void setColor(const aci::Color& _color) = 0;

		void print(const char * _str) { print(std::string(_str)); }
		void print(const wchar_t * _str) { print(std::wstring(_str)); }
		void setColor(int _r, int _g, int _b, int _a = 255) { setColor(aci::Color(_r, _g, _b, _a)); }
	};
}