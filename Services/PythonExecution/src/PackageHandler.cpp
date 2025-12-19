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
#include "ExceptionRestartRequired.h"
#include "OTCore/FolderNames.h"
#include "OTCore/EntityName.h"
#include "EntityResultText.h"

PackageHandler::~PackageHandler()
{
    delete m_currentManifest;
    m_currentManifest = nullptr;
}

void PackageHandler::initializeManifest(ot::UID _manifestUID)
{

    if (_manifestUID != ot::invalidUID)
    {
        if (m_environmentState == EnvironmentState::core)
        {
            //Here we have only the core instantiated and want to run a script within a custom environment
            requestRestart();
        }

        if (m_currentManifest == nullptr)
        {
            //This is the initial state.
            assert(m_environmentState == EnvironmentState::empty);
            m_currentManifest = loadManifestEntity(_manifestUID);
        }
        else 
        {
            //Here we have a potential switch of a manifest. We need to check if the manifest id has changed.
            std::unique_ptr<EntityPythonManifest>newManifest(loadManifestEntity(_manifestUID));
            if (m_currentManifest->getManifestID() != newManifest->getManifestID())
            {
	            requestRestart();
                OT_LOG_D("Manifest change requires interpreter restart.");
            }
        }
    }
}

void PackageHandler::initializeEnvironmentWithManifest(const std::string& _environmentPath)
{
	m_environmentPath = _environmentPath;
	m_workerWaiterState->m_isWorkDone = false; //Here we have to block other threads until the initialisation is done
    try
    {
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

        buildInstalledPackageMap(installedPackages);

        assert(m_currentManifest != nullptr);
        std::optional<std::list<std::string>> packagesInManifest = m_currentManifest->getManifestPackages();
        if (!packagesInManifest.has_value())
        {
            throw std::exception(("Invalid manifest: " + m_currentManifest->getName()).c_str());
        }
        else
        {
            if (packagesInManifest.value().size() == 0)
            {
                OT_LOG_D("Manifest" + m_currentManifest->getName() + " does not contain any packages.");
            }
            else
            {
                assert(m_environmentState == EnvironmentState::empty); //Otherwise the manifest UID should have been a different one

                OT_LOG_D("Initialize environment with manifest packages.");
                OutputPipeline::instance().setRedirectOutputMode(OutputPipeline::RedirectionMode::applicationRead);
                for (const std::string& packageName : packagesInManifest.value())
                {
                    if (!isPackageInstalled(packageName))
                    {
                        installPackage(packageName);
                        m_environmentState = EnvironmentState::firstFilling;
                    }
                }
                m_installationLog = OutputPipeline::instance().flushOutput();
                OutputPipeline::instance().setRedirectOutputMode(OutputPipeline::RedirectionMode::sendToServer);
                dropImportCache();
            }
        }

        m_workerWaiterState->m_isWorkDone = true;
        m_workerWaiterState->m_conditionalVariable.notify_all();
    }
    catch (const std::exception& e)
    {
		OT_LOG_E("Failed to initialize environment: " + std::string(e.what()));
        Application::instance().getCommunicationHandler().writeToServer("Failed to initialize environment: " + std::string(e.what()));
        exit(0);
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

    if (m_uninstalledPackages.size() != 0)
    {

        if (m_environmentState == EnvironmentState::initialised)
        {
            //Here we have an initialised environment, so we need to restart the interpreter with a new environment
            //In case that we have a named environment like Pyrit (not connected) we can not extend the environment
            if (m_currentManifest == nullptr)
            {
                throw std::exception("Packages can only be added if the execution is happening in a custom environment. Select an environment and run again.");
            }
            

            ot::UID newManifestUID = m_currentManifest->generateNewManifestID();
            //std::filesystem::path sourcePath(m_environmentPath);
            //std::filesystem::path targetPath = sourcePath.parent_path() / std::to_string(newManifestUID);
            ////std::filesystem::copy(sourcePath, targetPath);
            //m_environmentPath = targetPath.string();
            //for (const std::string& packageName : m_uninstalledPackages)
            //{
            //    installPackage(packageName);
            //}
            //dropImportCache();
            //We still need to update the manifest text. But we won't get it with the pip freeze command since we are still in the old environment

            m_currentManifest->storeToDataBase();
            ot::NewModelStateInfo newModelStateInfo;
            newModelStateInfo.addTopologyEntity(*m_currentManifest);
            ot::ModelServiceAPI::addEntitiesToModel(newModelStateInfo, "Manifest requires a new environment");

            requestRestart();
        }
        else if (m_environmentState == EnvironmentState::core)
        {
            throw std::exception("Packages can only be added if the execution is happening in a custom environment. Select an environment and run again.");
            //EntityPythonManifest newManifest;
            //std::list<std::string> folderContent = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::PythonManifestFolder);
            //const std::string entityName = ot::EntityName::createUniqueEntityName(ot::FolderNames::PythonManifestFolder, "Environment", folderContent);
            //newManifest.setName(entityName);
            //newManifest.setEntityID(Application::instance().getUIDGenerator()->getUID());
            //newManifest.storeToDataBase();

            
            //Needs to hand back a new manifest UID for the execution request
            //requestRestart();
        }
        else
        {
            OutputPipeline::instance().setRedirectOutputMode(OutputPipeline::RedirectionMode::applicationRead);
            //Environment is not yet initialised, so we can just install the packages but we need to update the manifest
            for (const std::string& packageName : m_uninstalledPackages)
            {
                installPackage(packageName);
            }
            dropImportCache();
            m_installationLog = OutputPipeline::instance().flushOutput();
			
            //Update manifest
            ot::NewModelStateInfo newModelStateInfo;
			storeInstallationLog(newModelStateInfo);

            std::string newManifest = getListOfInstalledPackages();
            m_currentManifest->replaceManifest(newManifest);
            m_currentManifest->storeToDataBase();
            newModelStateInfo.addTopologyEntity(*m_currentManifest);
            ot::ModelServiceAPI::addEntitiesToModel(newModelStateInfo, "Manifest requires a new environment");

            buildInstalledPackageMap(newManifest);
            OutputPipeline::instance().setRedirectOutputMode(OutputPipeline::RedirectionMode::sendToServer);
        }
        m_uninstalledPackages.clear();
		m_installationLog.clear();
    }
    else
    {
        if (!m_installationLog.empty())
        {
            ot::NewModelStateInfo newModelStateInfo;
			storeInstallationLog(newModelStateInfo);
            ot::ModelServiceAPI::addEntitiesToModel(newModelStateInfo, "Installation log updated");
			m_installationLog.clear();
        }
    }

}

ot::UID PackageHandler::getManifestUID()
{
    if(m_currentManifest != nullptr)
    {
        return m_currentManifest->getManifestID();
	}
    else
    {
        return ot::invalidUID;
    }
}

void PackageHandler::requestRestart()
{
    throw ExceptionRestartRequired();
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
            
            //we may have a statement like import numpy as np
            auto positionOfAs = rest.find(" as ");
            rest = rest.substr(0, positionOfAs);

            // handle multiple imports
            auto modules = ot::String::split(rest, ','); 
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
    Application::instance().getCommunicationHandler().writeToServer("OUTPUT:Installing package: " + _packageName + "\n");
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
            else
            {
                Application::instance().getCommunicationHandler().writeToServer("OUTPUT:Installing completed\n");
            }
        }
        else {
            
            PyErr_Print();
        }
    }
}

void PackageHandler::dropImportCache()
{
    CPythonObjectNew importlib = PyImport_ImportModule("importlib");
    PyObject_CallMethod(importlib, "invalidate_caches", nullptr);
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

void PackageHandler::storeInstallationLog(ot::NewModelStateInfo& _newState)
{
    EntityResultText installLogEntity;
    installLogEntity.setEntityID(Application::instance().getUIDGenerator()->getUID());
    installLogEntity.setName(ot::FolderNames::PythonFolder + "/Installation log");
    installLogEntity.setText(m_installationLog);
    installLogEntity.storeToDataBase();


    _newState.addTopologyEntity(installLogEntity);
    _newState.addDataEntity(installLogEntity.getEntityID(), installLogEntity.getTextDataStorageId(), installLogEntity.getTextDataStorageVersion());
    m_installationLog.clear();
}

void PackageHandler::buildInstalledPackageMap(const std::string& _packageList)
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