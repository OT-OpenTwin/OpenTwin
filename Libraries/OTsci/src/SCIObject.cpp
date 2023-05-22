//! @file OTsciAPIExport.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#include <aci/InterpreterObject.h>
#include <aci/InterpreterCore.h>
#include <aci/AbstractPrinter.h>
#include <aci/SCIFile.h>
#include <aci/SCIDir.h>
#include <aci/OS.h>
#include <aci/Convert.h>
#include <aci/AbstractInterpreterNotifier.h>

ot::InterpreterObject::InterpreterObject() : m_printer(nullptr), m_core(nullptr), m_notifier(nullptr) {}

// ################################################################################################################################

// Output

void ot::InterpreterObject::showInfo(void) {
	printDelimiterLine();
	print("###  ");
	setColor(240, 70, 140);
	print(key());
	setColor(255, 255, 255);
	print("  ###\n");
	printDelimiterLine();
	print("\n");
	showCommandInfo();
}

void ot::InterpreterObject::printDelimiterLine(void) {
	setColor(0, 255, 0);
	print("####################################################################################################\n");
	setColor(255, 255, 255);
}

// ################################################################################################################################

// File operations

bool ot::InterpreterObject::readDatSCIFile(std::wstring& _data, const std::wstring& _filename, bool _showLog) {
	std::wstring filePath = OS::instance()->handler()->scriptDatSCIDirectory() + L"/" + key() + L"/" + _filename;
	return OS::instance()->handler()->readFile(_data, filePath);
}

bool ot::InterpreterObject::readDatSCIFile(std::list<std::wstring>& _data, const std::wstring& _filename, bool _showLog) {
	std::wstring filePath = OS::instance()->handler()->scriptDatSCIDirectory() + L"/" + key() + L"/" + _filename;
	return OS::instance()->handler()->readLinesFromFile(_data, filePath);
}

bool ot::InterpreterObject::writeDatSCIFile(const std::wstring& _data, const std::wstring& _filename, bool _showLog) {
	std::wstring filePath = OS::instance()->handler()->scriptDatSCIDirectory() + L"/" + key() + L"/" + _filename;
	for (size_t i{ 0 }; i < filePath.length(); i++) {
		if (filePath.at(i) == L'\\') { filePath.replace(i, 1, L"/"); }
	}
	return OS::instance()->handler()->writeFile(_data, filePath);
}

bool ot::InterpreterObject::writeDatSCIFile(const std::list<std::wstring>& _data, const std::wstring& _filename, bool _showLog) {
	std::wstring filePath = OS::instance()->handler()->scriptDatSCIDirectory() + L"/" + key() + L"/" + _filename;
	for (size_t i{ 0 }; i < filePath.length(); i++) {
		if (filePath.at(i) == L'\\') { filePath.replace(i, 1, L"/"); }
	}
	return OS::instance()->handler()->writeLinesToFile(_data, filePath);
}

// ################################################################################################################################

// Setter

void ot::InterpreterObject::disableInput(void) {
	if (m_notifier) m_notifier->disableInput();
}

void ot::InterpreterObject::enableInput(void) {
	if (m_notifier) {
		m_notifier->enableInput();
	}
}

void ot::InterpreterObject::print(const char * _message) { print(std::string(_message)); }
void ot::InterpreterObject::print(const wchar_t * _message) { print(std::wstring(_message)); }

void ot::InterpreterObject::print(bool _value) {
	if (_value) {
		setColor(0, 255, 0);
		print(L"TRUE");
		setColor(255, 255, 255);
	}
	else {
		setColor(255, 0, 0);
		print(L"FALSE");
		setColor(255, 255, 255);
	}
}

void ot::InterpreterObject::print(const std::string& _message) {
	if (m_printer) { m_printer->print(_message); }
}
void ot::InterpreterObject::print(const std::wstring& _message) {
	if (m_printer) { m_printer->print(_message); }
}

void ot::InterpreterObject::setColor(int _r, int _g, int _b, int _a) {
	setColor(Color(_r, _g, _b, _a));
}
void ot::InterpreterObject::setColor(const Color& _color) {
	if (m_printer) { m_printer->setColor(_color); }
}
void ot::InterpreterObject::setColor(Color::DefaultColor _color) { setColor(Color(_color)); }

// ################################################################################################################################

// Getter

std::list<std::wstring> ot::InterpreterObject::filesInDirectory(const std::wstring& _directoryPath) {
	AbstractOSHandler * os = OS::instance()->handler();
	return os->filesInDirectory(_directoryPath, true);
}
std::list<std::wstring> ot::InterpreterObject::filesInDatSCIDirectory(const std::wstring& _subdirectory) {
	AbstractOSHandler * os = OS::instance()->handler();
	return os->filesInDirectory(os->scriptDatSCIDirectory() + L"/" + key() + L"/" + _subdirectory, true);
}

std::list<std::wstring> ot::InterpreterObject::subdirectories(const std::wstring& _directoryPath) {
	AbstractOSHandler * os = OS::instance()->handler();
	return os->subdirectories(_directoryPath, true);
}
std::list<std::wstring> ot::InterpreterObject::subdirectoriesInDatSCIDirectory(const std::wstring& _subdirectory) {
	AbstractOSHandler * os = OS::instance()->handler();
	return os->subdirectories(os->scriptDatSCIDirectory() + L"/" + key() + L"/" + _subdirectory, true);
}

// ################################################################################################################################

// Static functions

std::wstring ot::InterpreterObject::isolateFilename(const std::wstring& _path) {
	std::wstring path = _path;
	for (size_t i{ 0 }; i < path.length(); i++) {
		if (path.at(i) == L'\\') { path.replace(i, 1, L"/"); }
	}
	std::list<std::wstring> lst = splitString(path, L"/");
	std::list<std::wstring> filename = splitString(lst.back(), L".");
	std::wstring file;
	size_t ct{ 0 };
	for (auto f : filename) {
		if (++ct != filename.size()) {
			if (!file.empty()) { file.append(L"."); }
			file.append(f);
		}
	}
	return file;
}

