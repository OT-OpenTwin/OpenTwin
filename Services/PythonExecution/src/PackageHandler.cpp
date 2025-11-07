#include "PackageHandler.h"
#include <sstream>
#include "OTCore/String.h"
#include <Python.h>
#include "CPythonObjectNew.h"

void PackageHandler::importMissingPackages(const std::string _scriptContent)
{
	std::list<std::string> moduleNames = parseImportedPackages(_scriptContent);
    for(const std::string moduleName : moduleNames)
    {
        if (!isPackageInstalled(moduleName))
        {
            installPackage(moduleName);
        }
	}
}

const std::list<std::string> PackageHandler::parseImportedPackages(const std::string _scriptContent)
{
	std::list<std::string> packageList;
	std::stringstream scriptStream(_scriptContent);
    std::string line;
	while(std::getline(scriptStream, line)) 
    {
        line = trim(line);
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#')
        {
            continue;
        }
        // Check for "import ..." statements
        if (line.find("import ") == 0) {
            std::string rest = line.substr(7); // after "import "
            
            auto modules = ot::String::split(rest, ','); // handle multiple imports
			packageList.insert(packageList.end(), modules.begin(), modules.end());
        }
        // Check for "from ... import ..." statements
        else if (line.find("from ") == 0) {
            // Example: "from os.path import join"
            size_t pos = line.find("import ");
            if (pos != std::string::npos) {
                std::string module = trim(line.substr(5, pos - 5)); // after "from " before "import"
                if (!module.empty())
                {
                    packageList.push_back(module);
                }
            }
        }
	}
    
	return packageList;
}

 std::string PackageHandler::trim(const std::string& _line) {
    size_t start = _line.find_first_not_of(" \t");
    if (start == std::string::npos) return "";
    size_t end = _line.find_last_not_of(" \t");
    return _line.substr(start, end - start + 1);
}


bool PackageHandler::isPackageInstalled(const std::string& _packageName)
{
    CPythonObjectNew module = PyImport_ImportModule(_packageName.c_str());
    if (module) 
    {
        return true;  // Module exists
    }
    else {
        PyErr_Clear(); // Clear import error
        return false; // Module not found
    }
}

void PackageHandler::installPackage(const std::string& _packageName)
{
    // Import sys and set sys.argv
    PyObject* sys_module = PyImport_ImportModule("sys");
    PyObject* sys_argv = PyObject_GetAttrString(sys_module, "argv");

    // Build argv = ['pip', 'install', 'package', '--target=target_path']
    PyObject* argv = Py_BuildValue("[s,s,s,s]",
        "pip",
        "install",
        _packageName.c_str(),
        ("--target=" + m_targetPath).c_str());

    PyObject_SetAttrString(sys_module, "argv", argv);
    Py_DECREF(argv);

    // Run pip as a module
    PyRun_SimpleString(
        "import runpy\n"
        "runpy.run_module('pip', run_name='__main__', alter_sys=True)\n"
    );

    // Restore original sys.argv
    PyObject_SetAttrString(sys_module, "argv", sys_argv);
    Py_DECREF(sys_argv);
    Py_DECREF(sys_module);
}
