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
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityAPI.h"
#include "EntityPythonManifest.h"
#include "Application.h"
#include "OutputPipeline.h"

#include <filesystem>
#include "PythonObjectBuilder.h"

PackageHandler::~PackageHandler()
{
    delete m_currentManifest;
    m_currentManifest = nullptr;
}

void PackageHandler::initializeManifest(ot::UID _manifestUID)
{
    if (_manifestUID == ot::invalidUID)
    {
        throw std::exception("Invalid manifest UID provided.");
    }

    if (m_currentManifest == nullptr)
    {
        m_currentManifest = loadManifestEntity(_manifestUID);
    }
    else 
    {
        std::unique_ptr<EntityPythonManifest>newManifest(loadManifestEntity(_manifestUID));
        if (m_currentManifest->getManifestID() != newManifest->getManifestID())
        {
	        requestRestart();
            OT_LOG_D("Manifest change requires interpreter restart.");
        }
    }
    
}

void PackageHandler::initializeEnvironmentWithManifest(const std::string& _environmentPath)
{
	m_environmentPath = _environmentPath;

    //In this case we are in the process of a freshly started interpreter
    //First check if the environment exists and is initialised
    bool environmentExists = std::filesystem::is_directory(m_environmentPath);
    if (environmentExists)
    {
        //If the folder does not contains packages, we need to install them. 
        if (std::filesystem::is_empty(m_environmentPath))
        {
			m_environmentState = EnvironmentState::empty;
        }
        else
        {
            m_environmentState = EnvironmentState::initialised;
        }
    }
    else
    {
        m_environmentState = EnvironmentState::empty;
    }
    OutputPipeline::instance().setRedirectOutputMode(OutputPipeline::RedirectionMode::applicationRead);
    const std::string installedPackages = getListOfInstalledPackages();
    OutputPipeline::instance().setRedirectOutputMode(OutputPipeline::RedirectionMode::sendToServer);

    buildPackageMap(installedPackages);

	assert(m_currentManifest != nullptr);
    std::optional<std::list<std::string>> packagesInManifest = m_currentManifest->getManifestPackages();
    if (!packagesInManifest.has_value())
    {
        throw std::exception("Invalid manifest");
    }
    else
    {
        if(packagesInManifest.value().size() == 0)
        {
            OT_LOG_D("Manifest does not contain any packages.");
	    }
        else
        {
            assert(m_environmentState == EnvironmentState::empty); //Otherwise the manifest UID should have been a different one

            OT_LOG_D("Initialize environment with manifest packages.");
            for (const std::string& packageName : packagesInManifest.value())
            {
                if (!isPackageInstalled(packageName))
                {
                    installPackage(packageName);
                    m_environmentState = EnvironmentState::firstFilling;
                }
            }
        }
    }
}

void PackageHandler::extractMissingPackages(const std::string& _scriptContent)
{
    std::list<std::string> moduleNames = parseImportedPackages(_scriptContent);
    
    for (const std::string moduleName : moduleNames)
    {
        if (!isPackageInstalled(moduleName))
        {
            m_uninstalledPackages.push_back(moduleName);
        }
    }
}

void PackageHandler::importMissingPackages()
{
    if (m_uninstalledPackages.size() != 0 && m_currentManifest == nullptr)
    {
        // In this case we have no defined manifest and the Pyrit environment is used. An extension is currently not supported.
        m_uninstalledPackages.clear();
        throw std::exception("The script uses packages that are not part of the pyrit environment. An extension is currently not supported.");
    }
    else if (m_uninstalledPackages.size() != 0)
    {
        OutputPipeline::instance().setRedirectOutputMode(OutputPipeline::RedirectionMode::applicationRead);
        if (m_environmentState == EnvironmentState::initialised)
        {
            //Here we have an initialised environment, so we need to restart the interpreter with a new environment
            //First we copy the current environment and give it the name of the new manifest
            ot::UID newManifestUID = m_currentManifest->generateNewManifestID();
            std::filesystem::path sourcePath(m_environmentPath);
            std::filesystem::path targetPath = sourcePath.parent_path() / std::to_string(newManifestUID);
            std::filesystem::copy(sourcePath, targetPath);
            m_environmentPath = targetPath.string();
            for (const std::string& packageName : m_uninstalledPackages)
            {
                installPackage(packageName);
            }
            //We still need to update the manifest text. But we won't get it with the pip freeze command since we are still in the old environment

            m_currentManifest->storeToDataBase();
            ot::NewModelStateInfo newModelStateInfo;
            newModelStateInfo.addTopologyEntity(*m_currentManifest);
            ot::ModelServiceAPI::addEntitiesToModel(newModelStateInfo, "Manifest requires a new environment");

            requestRestart();
        }
        else
        {
            //Environment is not yet initialised, so we can just install the packages but we need to update the manifest
            for (const std::string& packageName : m_uninstalledPackages)
            {
                installPackage(packageName);
            }
            const std::string installLog = OutputPipeline::instance().flushOutput();
            //Update manifest
            std::string newManifest = getListOfInstalledPackages();
            m_currentManifest->replaceManifest(newManifest);
            m_currentManifest->storeToDataBase();
            ot::NewModelStateInfo newModelStateInfo;
            newModelStateInfo.addTopologyEntity(*m_currentManifest);
            ot::ModelServiceAPI::addEntitiesToModel(newModelStateInfo, "Manifest requires a new environment");

            buildPackageMap(newManifest);
        }
        m_uninstalledPackages.clear();
        OutputPipeline::instance().setRedirectOutputMode(OutputPipeline::RedirectionMode::sendToServer);
    }

}

std::string PackageHandler::getEnvironmentName()
{
    if(m_currentManifest != nullptr)
    {
        return std::to_string(m_currentManifest->getManifestID());
	}
    else
    {
        return std::string();
    }
}

void PackageHandler::requestRestart()
{
    Application::instance().getCommunicationHandler().writeToServer("Restart");
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

 std::pair<std::string, std::string> PackageHandler::splitPackageIntoNameAndVersion(const std::string& _requirementLine)
 {
     const std::string delim = "==";
     size_t pos = _requirementLine.find(delim);

     if (pos != std::string::npos)
     {
        std::string package = _requirementLine.substr(0, pos);
        std::string version = _requirementLine.substr(pos + delim.length());
        
        auto pair = std::make_pair(package, version);
        return pair;
     }
     else
     {
        return std::make_pair("", "");
     }
 }

bool PackageHandler::isPackageInstalled(const std::string& _packageName)
{
    //First option, does not work with packages like tensorflow
    CPythonObjectNew temp = PyImport_ImportModule(_packageName.c_str());
    bool importableModule = temp != nullptr;
    if (importableModule)
    {
        return true;
    }
    else
    {
        //Alternative way checks the list of pip insstalled packages.
        auto nameAndVersion = splitPackageIntoNameAndVersion(_packageName);
        if (!nameAndVersion.first.empty())
        {
            auto installedPackage = m_installedPackageVersionsByName.find(nameAndVersion.first);
            if (installedPackage != m_installedPackageVersionsByName.end())
            {
                bool fits = installedPackage->second == nameAndVersion.second;
                return fits;
            }
            else
            {
                return false;
            }
        }
        else
        {
            bool found = m_installedPackageVersionsByName.find(_packageName) != m_installedPackageVersionsByName.end();
            return found;
        }
    }
}

void PackageHandler::installPackage(const std::string& _packageName)
{
    Application::instance().getCommunicationHandler().writeToServer("OUTPUT:Installing package: " + _packageName);
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
        ("--target=" + m_environmentPath).c_str()
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

EntityPythonManifest* PackageHandler::loadManifestEntity(ot::UID _manifestUID)
{
    ot::EntityInformation info;
    ot::ModelServiceAPI::getEntityInformation(_manifestUID, info);
    EntityBase* entityBase = ot::EntityAPI::readEntityFromEntityIDandVersion(info);
    if(entityBase == nullptr)
    {
        throw std::exception("Failed to load manifest entity from database.");
	}
    return dynamic_cast<EntityPythonManifest*>(entityBase);        
}

ot::UID PackageHandler::getUIDFromString(const std::string& _uid)
{
    try
    {
        unsigned long long temp = std::stoull(_uid);
        ot::UID manifestUID = static_cast<ot::UID>(temp);
        return manifestUID;
    }
    catch (std::exception&)
    {
        return ot::invalidUID;
    }
}

std::string PackageHandler::getListOfInstalledPackages()
{
    CPythonObjectNew sys_module = PyImport_ImportModule("sys");
    if (!sys_module)
    {
        throw std::exception("Failed to import sys module");
    }

    // sys.argv = ["pip", "freeze"]
    CPythonObjectNew argv = PyList_New(2);
    PyList_SetItem(argv, 0, PyUnicode_FromString("pip"));
    PyList_SetItem(argv, 1, PyUnicode_FromString("freeze"));

    if (PyObject_SetAttrString(sys_module, "argv", argv) != 0)
    {
        throw std::exception("Failed to set sys.argv");
    }

    // Import runpy
    CPythonObjectNew runpy = PyImport_ImportModule("runpy");
    if (!runpy)
    {
        throw std::exception("Failed to import runpy");
    }

    CPythonObjectNew run_module = PyObject_GetAttrString(runpy, "run_module");
    if (!run_module)
    {
        throw std::exception("Failed to get run_module");
    }

    // run_module('pip.__main__', run_name='__main__', alter_sys=True)
    CPythonObjectNew args = Py_BuildValue("(s)", "pip.__main__");
    CPythonObjectNew kwargs = Py_BuildValue(
        "{s:s, s:O}",
        "run_name", "__main__",
        "alter_sys", Py_True
    );

    CPythonObjectNew result = PyObject_Call(run_module, args, kwargs);
	std::string allInstalledPackages = OutputPipeline::instance().flushOutput();
    // Extract the "result" variable (a Python list)

        /*for(const std::string& pkg : installedPackages)
        {
            allInstalledPackages += pkg + "\n";
        }*/

    // Check if the error was SystemExit
    if (PyErr_ExceptionMatches(PyExc_SystemExit))
    {
        // Clear the error and continue
        PyErr_Clear();
        // Pip ran successfully but exited the interpreter
    }
    else
    {
        PyErr_Print();
        throw std::exception("pip execution failed");
    }

    return allInstalledPackages;
}

void PackageHandler::buildPackageMap(const std::string& _packageList)
{
    std::stringstream stringStream(_packageList);
    std::string line;
    const std::string delim = "==";
    while (std::getline(stringStream, line))
    {
        auto nameAndVersion = splitPackageIntoNameAndVersion(line);
        if (!nameAndVersion.first.empty())
        {
            m_installedPackageVersionsByName[nameAndVersion.first] = nameAndVersion.second;
        }
    }
}