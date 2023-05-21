//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include <aci/InterpreterCore.h>
#include <aci/AbstractInterpreterNotifier.h>
#include <aci/InterpreterObject.h>
#include <aci/AbstractPrinter.h>
#include <aci/aFile.h>
#include <aci/aDir.h>
#include <aci/Convert.h>
#include <aci/OS.h>
#include <aci/ScriptLoader.h>

using namespace aci;

InterpreterCore * g_instance{ nullptr };

InterpreterCore * InterpreterCore::instance(void) {
	if (g_instance == nullptr) { g_instance = new InterpreterCore; }
	return g_instance;
}

void InterpreterCore::clearInstance(void) {
	if (g_instance) {
		delete g_instance;
		g_instance = nullptr;
	}
}

// ########################################################################################

// Setter

void InterpreterCore::attachPrinter(aci::AbstractPrinter * _printer) {
	m_printer = _printer;
	for (auto obj : m_objects) { obj.second->attachPrinter(m_printer); }
}

void InterpreterCore::addScriptObject(aci::InterpreterObject * _obj) {
	if (_obj == nullptr) {
		if (m_printer) { m_printer->print("Failed to load NULL as script\n"); }
		return;
	}
	auto h = m_objects.find(_obj->key());
	if (h != m_objects.end()) {
		if (m_printer) { m_printer->print(L"Script \"" + _obj->key() + L"\" was already loaded\n"); }
		return;
	}
	m_objects.insert_or_assign(_obj->key(), _obj);
	_obj->attachCore(this);
	_obj->attachNotifier(m_notifier);
	_obj->attachPrinter(m_printer);
}

void InterpreterCore::removeScriptObject(const std::wstring& _key, bool _deleteObject) {
	auto it = m_objects.find(_key);
	if (it == m_objects.end()) return;
	if (_deleteObject) delete it->second;
	m_objects.erase(_key);
}

void InterpreterCore::setCurrentPath(const std::wstring& _path) {
	m_path.clear();
	if (_path.empty()) { return; }
	std::wstring sub(_path);
	for (size_t i{ 0 }; i < sub.length(); i++) {
		if (sub.at(i) == L'\\') { sub.replace(i, 1, L"/"); }
	}
	std::list<std::wstring> lst = splitString(sub, L"/");
	for (auto s : lst) { m_path.push_back(s); }
}

// ########################################################################################

// Event handler

bool InterpreterCore::handle(const std::wstring& _message) {
	std::vector<std::wstring> lst;
	extractClassicSyntax(lst, _message);

	if (_message == L"?") {
		showHelp();
		return true;
	}
	else if (_message == L"exit" || _message == L"end") {
		if (m_printer) { m_printer->print(L"Shutdown requested\n"); }
		if (m_notifier) { m_notifier->shutdown(); return true; }
		else if (m_printer) {
			m_printer->setColor(255, 0, 0);
			m_printer->print("No notifier attached\n");
		}
		return false;
	}
	else if (_message == L"?exit" || _message == L"?end") {
		if (m_printer) {
			m_printer->print(L"The command \"exit\" or \"end\" will close the application\n");
		}
		return true;
	}
	else if (_message.find(L"$data") == 0) {
		return cmdData(lst);
	}
	else if (lst.front() == L"cd") {
		if (lst.size() == 1) {
			return cmdCd(L"~");
		}
		else if (lst.size() > 2 || lst.size() < 2) {
			if (m_printer) {
				m_printer->setColor(255, 0, 0);
				m_printer->print(L"Invalid nuber of arguments for \"cd\"\n");
			}
			return false;
		}
		else {
			return cmdCd(lst.back());
		}
	}
	else if (_message == L"ls") {
		if (m_printer == nullptr) { return true; }
		if (m_path.empty()) {
			m_printer->print(L"\t<empty>\n");
			return true;
		}
		aci::aDir dir(L"", currentPath());
		dir.scanDirectories();
		dir.scanFiles();

		m_printer->print(dir.fullPath() + L"\n");

		m_printer->setColor(255, 150, 50);
		for (auto d : dir.subdirectories()) {
			m_printer->print(L"\t[DIR]:  " + d->name() + L"\n");
		}
		m_printer->setColor(50, 255, 150);
		for (auto f : dir.files()) {
			m_printer->print(L"\t[FILE]: " + f->name() + L"\n");
		}
		return true;
	}
	else if (lst.size() == 1 && lst.front().find(L"?") == 0) {
		std::wstring key = lst.front();
		key.erase(0, 1);
		auto handler = m_objects.find(key);
		if (handler != m_objects.end()) {
			handler->second->showInfo();
			return true;
		}
		else if (m_printer) {
			m_printer->setColor(255, 0, 0);
			m_printer->print(L"Command \"");
			m_printer->setColor(200, 200, 0);
			m_printer->print(key);
			m_printer->setColor(255, 0, 0);
			m_printer->print(L"\" was not recognized\n");
		}
		return false;
	}
	else {
		auto handler = m_objects.find(lst.front());
		if (handler != m_objects.end()) {
			return handler->second->handle(_message, lst);
		} else if (m_printer) { 
			m_printer->setColor(255, 0, 0);
			m_printer->print(L"Command \"");
			m_printer->setColor(200, 200, 0);
			m_printer->print(lst.front());
			m_printer->setColor(255, 0, 0);
			m_printer->print(L"\" was not recognized\n");
		}
		return false;
	}
}

bool InterpreterCore::cmdData(const std::vector<std::wstring>& _args) {
	if (_args.empty()) {
		if (m_printer) { m_printer->print(L"[ERROR] [INTERPRETER] [CMD]: Invalid number of arguments\n"); }
		return false;
	}
	if (_args.size() == 1) {
		if (m_printer) {
			m_printer->print(L"[INTERPRETER] [DATA]: Registered commands\n");
			m_printer->print(L"\t?\n");
			m_printer->print(L"\t$data\n");
			m_printer->print(L"\texit\n\tend\n");
			m_printer->print(L"\tcd\n");
			for (auto obj : m_objects) {
				m_printer->print(L"\t" + obj.first + L"\n");
			}
		}
		return true;
	}
	else {
		if (m_printer) { m_printer->print(L"[ERROR] [INTERPRETER] [CMD]: Invalid number of arguments\n"); }
		return false;
	}
}

void InterpreterCore::showHelp(void) {
	if (m_printer) {
		m_printer->print(L"################################\n");
		m_printer->print(L"Core commands\n");
		m_printer->print(L"\t$data		display all datasets\n");
		m_printer->print(L"\t?<command>  display the command help\n");
		m_printer->print(L"################################\n");
	}
}

bool InterpreterCore::cmdCd(const std::wstring& _path) {
	if (_path == L"?") {
		if (m_printer) {
			m_printer->print("########################################\n## cd ##\n\n");
			m_printer->print("\tcd ?                 Show this help\n");
			m_printer->print("\tcd ~  -OR-  cd       Show the current configuration\n");
			m_printer->print("\tcd <directory>       Change the directory to the directory specified\n");
			m_printer->print("\tcd -                 Go up one directory\n");
			m_printer->print("\tcd ??                Print the current directory\n");
		}
		return true;
	}
	if (_path.empty() || _path == L"~") {
		m_path.clear();
		return true;
	}
	else if (_path == L"-") {
		if (!m_path.empty()) { m_path.pop_back(); }
		return true;
	}
	else if (_path == L"??") {
		if (m_printer) {
			m_printer->print(currentPath() + L"\n");
		}
		return true;
	}
	else {
		if (!m_path.empty()) {
			std::wstring subDir = currentPath() + L"/" + _path;
			aDir currentDir(L"", subDir);
			if (currentDir.exists()) {
				setCurrentPath(currentPath() + L"/" + _path);
				return true;
			}
		}

		aDir dir(L"", _path);
		if (dir.exists()) {
			setCurrentPath(_path);
			return true;
		}

		if (m_printer) {
			m_printer->setColor(255, 0, 0);
			m_printer->print(L"Directory not found\n");
		}

		return false;
	}
}

// ########################################################################################

// Getter

std::wstring InterpreterCore::currentPath(void) const {
	if (m_path.empty()) { return L"root"; }
	std::wstring path;
	for (std::wstring p : m_path) {
		if (!path.empty()) { path.append(L"/"); }
		path.append(p);
	}
	return path;
}

aci::InterpreterObject * InterpreterCore::findFirstMatchingItem(const std::wstring& _key) {
	auto it = m_objects.find(_key);
	if (it != m_objects.end()) {
		return it->second;
	}
	for (auto it : m_objects) {
		if (it.second->key().find(_key) == 0) { return it.second; }
	}
	return nullptr;
}

// ########################################################################################

// Static functions

void InterpreterCore::extractClassicSyntax(std::vector<std::wstring>& _dest, const std::wstring& _origin) {
	bool isString{ false };
	std::wstring temp;
	wchar_t arr[2];
	arr[1] = 0;
	for (wchar_t c : _origin) {
		if (c == L' ') {
			if (isString) {
				arr[0] = c;
				temp.append(arr);
			}
			else { _dest.push_back(temp); temp.clear(); }
		}
		else if (c == '\"') { isString = !isString; }
		else {
			arr[0] = c;
			temp.append(arr);
		}
	}
	if (!temp.empty()) { _dest.push_back(temp); }
}

// ########################################################################################

// Private functions

InterpreterCore::InterpreterCore()
	: m_printer(nullptr), m_autoClean(true), m_notifier(nullptr)
{
	setCurrentPath(OS::instance()->handler()->currentDirectory());
	m_scriptLoader = new ScriptLoader(this);
}

InterpreterCore::~InterpreterCore() {
	if (m_autoClean) {
		for (auto obj : m_objects) { delete obj.second; }
	}
	delete m_scriptLoader;
}