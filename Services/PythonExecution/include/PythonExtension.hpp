// @otlicense
// File: PythonExtension.hpp
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

#pragma once

#include "PythonExtension.h"
#include "PythonException.h"
#include "PythonModuleAPI.h"
#include "PythonLoadedModules.h"
#include "PythonObjectBuilder.h"
#include "EntityBuffer.h"
#include "DataBuffer.h"

#include "EntityFile.h"

PyObject* PythonExtensions::OT_GetPropertyValue(PyObject* _self, PyObject* _args) 
{
    try
    {
        CPythonObjectBorrowed args(_args);
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 2;
        if (numberOfArguments != expectedNumberOfArguments) {
            throw std::exception("OT_GetPropertyValue expects two arguments");
        }
        PythonObjectBuilder pyObBuilder;
        std::string absoluteEntityName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");
        std::string propertyName = pyObBuilder.getStringValueFromTuple(args, 1, "Parameter 1");

        PyObject* returnValue = EntityBuffer::instance().getEntityPropertyValue(absoluteEntityName, propertyName);
        return returnValue;
    }
    catch (std::exception& _e)
    {
        PyErr_SetString(PyExc_RuntimeError, _e.what());
        return nullptr;
    }
}

PyObject* PythonExtensions::OT_GetScript(PyObject* _self, PyObject* _args) 
{    
    try
    {
        CPythonObjectBorrowed args(_args);

        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 1;
        if (numberOfArguments != expectedNumberOfArguments) {
            throw std::exception("OT_GetScript expects one argument");
        }
        PythonObjectBuilder pyObBuilder;
        std::string absoluteScriptName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");


        auto baseEntity = EntityBuffer::instance().getEntity(absoluteScriptName);
        ot::EntityInformation entityInfo(baseEntity.get());
        std::optional<std::string> moduleName = PythonLoadedModules::instance().getModuleName(entityInfo);
        CPythonObjectNew moduleImported(nullptr);

        if (!moduleName.has_value()) {
            EntityFile* script = dynamic_cast<EntityFile*>(baseEntity.get());
            if (script == nullptr) {
                throw std::exception("Requested script execution cannot be done, since the entity is not a python script.");
            }

            PythonLoadedModules::instance().addModuleForEntity(entityInfo);
            moduleName = PythonLoadedModules::instance().getModuleName(entityInfo);

            const std::vector<char>& plainData = script->getDataEntity()->getData();
            std::string execution(plainData.begin(), plainData.end());
            CPythonObjectBorrowed  newModule(PyImport_AddModule(moduleName.value().c_str()));
            moduleImported = PyImport_ImportModule(moduleName.value().c_str());

            CPythonObjectBorrowed globalDictionary(PyModule_GetDict(moduleImported));
            CPythonObjectNew result(PyRun_String(execution.c_str(), Py_file_input, globalDictionary, globalDictionary));
            if (result == nullptr) {
                throw PythonException();
            }

        }
        else {
            moduleImported = PyImport_ImportModule(moduleName.value().c_str());
        }

        std::string entryPoint = PythonModuleAPI::instance().getModuleEntryPoint(moduleImported);
        return PyObject_GetAttrString(moduleImported, entryPoint.c_str());
    }
    catch (std::exception& _e)
    {
        PyErr_SetString(PyExc_RuntimeError, _e.what());
        return nullptr;
    }
}

PyObject* PythonExtensions::OT_SetPropertyValue(PyObject* _self, PyObject* _args) 
{
    try
    {
        CPythonObjectBorrowed args(_args);
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 3;
        if (numberOfArguments != expectedNumberOfArguments) {
            throw std::exception("OT_SetPropertyValue expects three arguments");
        }

        PythonObjectBuilder pyObBuilder;
        std::string absoluteEntityName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");
        std::string propertyName = pyObBuilder.getStringValueFromTuple(args, 1, "Parameter 1");
        CPythonObjectBorrowed pvalue = pyObBuilder.getTupleItem(args, 2, "Parameter 2");

        EntityBuffer::instance().updateEntityPropertyValue(absoluteEntityName, propertyName, pvalue);
        return PyBool_FromLong(true);
    }
    catch (std::exception& _e)
    {
        PyErr_SetString(PyExc_RuntimeError, _e.what());
        return nullptr;
    }
}

PyObject* PythonExtensions::OT_Flush(PyObject* _self, PyObject* _args) 
{
    try
    {
        if (_args != nullptr) {
            throw std::exception("OT_Flush expects zero arguments");
        }

        EntityBuffer::instance().saveChangedEntities();
        PythonObjectBuilder pyObBuilder;
        return PyBool_FromLong(true);
    }
    catch (std::exception& _e)
    {
        PyErr_SetString(PyExc_RuntimeError, _e.what());
        return nullptr;
    }
}

PyObject* PythonExtensions::OT_FlushEntity(PyObject* _self, PyObject* _args) 
{
    try
    {

        CPythonObjectBorrowed args(_args);
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 1;
        if (numberOfArguments != expectedNumberOfArguments) {
            throw std::exception("OT_FlushEntity expects one argument");
        }
        EntityBuffer::instance().saveChangedEntities();
        PythonObjectBuilder pyObBuilder;
        return pyObBuilder.setBool(true);
    }
    catch (std::exception& _e)
    {
        PyErr_SetString(PyExc_RuntimeError, _e.what());
        return nullptr;
    }
}

PyObject* PythonExtensions::OT_GetTableCell(PyObject* _self, PyObject* _args) 
{
    try
    {
        CPythonObjectBorrowed args(_args);
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 3;
        if (numberOfArguments != expectedNumberOfArguments) {
            throw std::exception("OT_GetTableCell expects three arguments");
        }
        PythonObjectBuilder pyObBuilder;
        std::string absoluteEntityName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");
        int32_t row = pyObBuilder.getInt32ValueFromTuple(args, 1, "Parameter 1");
        int32_t column = pyObBuilder.getInt32ValueFromTuple(args, 2, "Parameter 2");

        if (row < 0 || column < 0)
        {
            throw std::exception("OT_GetTableCell requires positive indices");
        }

        PyObject* returnValue = EntityBuffer::instance().getTableCellValue(absoluteEntityName, static_cast<uint32_t>(row), static_cast<uint32_t>(column));
        return returnValue;
    }
    catch (std::exception& _e)
    {
        PyErr_SetString(PyExc_RuntimeError, _e.what());
        return nullptr;
    }
}

PyObject* PythonExtensions::OT_GetPortData(PyObject* _self, PyObject* _args) 
{
    try
    {
        CPythonObjectBorrowed args(_args);
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 1;
        if (numberOfArguments != expectedNumberOfArguments) {
            throw std::exception("OT_GetPortData expects one arguments");
        }
        PythonObjectBuilder pyObBuilder;
        std::string portName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");
        PyObject* returnValue = DataBuffer::instance().getPortData(portName);
        return returnValue;
    }
    catch (std::exception& _e)
    {
        PyErr_SetString(PyExc_RuntimeError, _e.what());
        return nullptr;
    }
}

PyObject* PythonExtensions::OT_GetPortMetaData(PyObject* _self, PyObject* _args)
{
    try
    {
        CPythonObjectBorrowed args(_args);
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 1;
        if (numberOfArguments != expectedNumberOfArguments) {
            throw std::exception("OT_GetPortData expects one arguments");
        }
        PythonObjectBuilder pyObBuilder;
        std::string portName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");
        PyObject* returnValue = DataBuffer::instance().getPortMetaData(portName);
        return returnValue;
    }
    catch (std::exception& _e)
    {
        PyErr_SetString(PyExc_RuntimeError, _e.what());
        return nullptr;
    }
}

PyObject* PythonExtensions::OT_SetPortData(PyObject* _self, PyObject* _args) 
{
    try
    {
        CPythonObjectBorrowed args(_args);
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 2;
        if (numberOfArguments != expectedNumberOfArguments) {
            throw std::exception("OT_GetPortData expects two argument");
        }
        PythonObjectBuilder pyObBuilder;
        std::string portName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");
        CPythonObjectBorrowed pvalue = pyObBuilder.getTupleItem(args, 1, "Parameter 1");
        std::string values = pyObBuilder.getStringValue(pvalue, "port data");
        DataBuffer::instance().overridePortData(portName, std::move(values));
        return PyBool_FromLong(true);
    }
    catch (std::exception& _e)
    {
        PyErr_SetString(PyExc_RuntimeError, _e.what());
        return nullptr;
    }
}

PyObject* PythonExtensions::OT_SetPortMetaData(PyObject* _self, PyObject* _args)
{
    try
    {
        CPythonObjectBorrowed args(_args);
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 2;
        if (numberOfArguments != expectedNumberOfArguments) {
            throw std::exception("OT_GetPortData expects two argument");
        }
        PythonObjectBuilder pyObBuilder;
        std::string portName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");
        CPythonObjectBorrowed pvalue = pyObBuilder.getTupleItem(args, 1, "Parameter 1");
        std::string values = pyObBuilder.getStringValue(pvalue, "port data");
        DataBuffer::instance().overridePortMetaData(portName, std::move(values));
        return PyBool_FromLong(true);
    }
    catch (std::exception& _e)
    {
        PyErr_SetString(PyExc_RuntimeError, _e.what());
        return nullptr;
    }
}
