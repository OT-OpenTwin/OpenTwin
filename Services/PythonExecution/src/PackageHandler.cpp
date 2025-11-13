// @otlicense
// File: PackageHandler.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "PackageHandler.h"
#include <sstream>
#include "OTCore/String.h"
#include <Python.h>
#include "CPythonObjectNew.h"
#include "OTCore/LogDispatcher.h"
#include "PythonException.h"

void PackageHandler::importMissingPackages(const std::string _scriptContent)
{
    if(m_targetPath.empty())
    {
        OT_LOG_E("Target path for package installation is not set.");
        return;
	}

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
    

    for (std::string& packageName : packageList)
    {
		ot::String::removeControlCharacters(packageName);
		ot::String::removeWhitespaces(packageName);
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

    // Import sys module
    CPythonObjectNew sys_module = PyImport_ImportModule("sys");
    if (!sys_module) 
    {
        throw std::exception("Failed to import sys module\n");
    }

    // Build sys.argv = ['pip', 'install', packageName, '--target=targetPath']
    CPythonObjectNew argv = Py_BuildValue(
        "[s,s,s,s]",
        "pip",
        "install",
        _packageName.c_str(),
        ("--target=" + m_targetPath).c_str()
    );

    PyObject_SetAttrString(sys_module, "argv", argv);
    
    // Import runpy
    CPythonObjectNew runpy = PyImport_ImportModule("runpy");
    if (!runpy) 
    {
        throw std::exception("Failed to import runpy\n");
    }

    CPythonObjectNew run_module = PyObject_GetAttrString(runpy, "run_module");
    if (!run_module) 
    {
        throw std::exception("Failed to get run_module\n");
    }

    // Prepare arguments and keyword arguments for run_module('pip', run_name='__main__', alter_sys=True)
    CPythonObjectNew args = Py_BuildValue("(s)", "pip");
    CPythonObjectNew kwargs = Py_BuildValue("{s:s,s:O}",
        "run_name", "__main__",
        "alter_sys", Py_True);

    CPythonObjectNew result = PyObject_Call(run_module, args, kwargs);

    // Check for SystemExit
    if (!result) 
    {
        if (PyErr_ExceptionMatches(PyExc_SystemExit)) 
        {
            std::string message = "Failed to pip install package";
            PyObject* exc, * val, * tb;
            PyErr_Fetch(&exc, &val, &tb);

            CPythonObjectNew code = PyObject_GetAttrString(val, "code");
            long exit_code = 0;
            if (code && code != Py_None) 
            {
                exit_code = PyLong_AsLong(code);
            }

            if(exit_code != 0)
            {
                message += " '" + _packageName + "' with exit code " + std::to_string(exit_code);
                throw PythonException(message);
            }
        }
        else {
            
            PyErr_Print();
        }
    }
}
