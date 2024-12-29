#pragma once

#include "PythonExtension.h"
#include "PythonException.h"
#include "PythonModuleAPI.h"
#include "PythonLoadedModules.h"
#include "PythonObjectBuilder.h"
#include "EntityBuffer.h"
#include "PortDataBuffer.h"

#include "EntityFile.h"

PyObject* PythonExtensions::OT_GetPropertyValue(PyObject* _self, PyObject* _args) {
    auto numberOfArguments = PyTuple_Size(_args);
    const int expectedNumberOfArguments = 2;
    if (numberOfArguments != expectedNumberOfArguments) {
        throw std::exception("OT_GetPropertyValue expects two arguments");
    }
    PythonObjectBuilder pyObBuilder;
    std::string absoluteEntityName = pyObBuilder.getStringValueFromTuple(_args, 0, "Parameter 0");
    std::string propertyName = pyObBuilder.getStringValueFromTuple(_args, 1, "Parameter 1");

    PyObject* returnValue = EntityBuffer::instance().getEntityPropertyValue(absoluteEntityName, propertyName);
    return returnValue;
}

PyObject* PythonExtensions::OT_GetScript(PyObject* _self, PyObject* _args) {
    auto numberOfArguments = PyTuple_Size(_args);
    const int expectedNumberOfArguments = 1;
    if (numberOfArguments != expectedNumberOfArguments) {
        throw std::exception("OT_GetScript expects one argument");
    }
    PythonObjectBuilder pyObBuilder;
    std::string absoluteScriptName = pyObBuilder.getStringValueFromTuple(_args, 0, "Parameter 0");


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

        auto plainData = script->getData()->getData();
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

PyObject* PythonExtensions::OT_SetPropertyValue(PyObject* _self, PyObject* _args) {
    auto numberOfArguments = PyTuple_Size(_args);
    const int expectedNumberOfArguments = 3;
    if (numberOfArguments != expectedNumberOfArguments) {
        throw std::exception("OT_SetPropertyValue expects three arguments");
    }

    PythonObjectBuilder pyObBuilder;
    std::string absoluteEntityName = pyObBuilder.getStringValueFromTuple(_args, 0, "Parameter 0");
    std::string propertyName = pyObBuilder.getStringValueFromTuple(_args, 1, "Parameter 1");
    CPythonObjectBorrowed pvalue = pyObBuilder.getTupleItem(_args, 2, "Parameter 2");

    EntityBuffer::instance().updateEntityPropertyValue(absoluteEntityName, propertyName, pvalue);
    return PyBool_FromLong(true);
}

PyObject* PythonExtensions::OT_Flush(PyObject* _self, PyObject* _args) {
    if (_args != nullptr) {
        throw std::exception("OT_Flush expects zero arguments");
    }

    EntityBuffer::instance().saveChangedEntities();
    PythonObjectBuilder pyObBuilder;
    return PyBool_FromLong(true);
}

PyObject* PythonExtensions::OT_FlushEntity(PyObject* _self, PyObject* _args) {
    auto numberOfArguments = PyTuple_Size(_args);
    const int expectedNumberOfArguments = 1;
    if (numberOfArguments != expectedNumberOfArguments) {
        throw std::exception("OT_FlushEntity expects one argument");
    }
    EntityBuffer::instance().saveChangedEntities();
    PythonObjectBuilder pyObBuilder;
    return pyObBuilder.setBool(true);
}

PyObject* PythonExtensions::OT_GetTableCell(PyObject* _self, PyObject* _args) {
    auto numberOfArguments = PyTuple_Size(_args);
    const int expectedNumberOfArguments = 3;
    if (numberOfArguments != expectedNumberOfArguments) {
        throw std::exception("OT_GetTableCell expects three arguments");
    }
    PythonObjectBuilder pyObBuilder;
    std::string absoluteEntityName = pyObBuilder.getStringValueFromTuple(_args, 0, "Parameter 0");
    int32_t row = pyObBuilder.getInt32ValueFromTuple(_args, 1, "Parameter 1");
    int32_t column = pyObBuilder.getInt32ValueFromTuple(_args, 2, "Parameter 2");

    if (row < 0 || column < 0)
    {
        throw std::exception("OT_GetTableCell requires positive indices");
    }

    PyObject* returnValue = EntityBuffer::instance().getTableCellValue(absoluteEntityName, static_cast<uint32_t>(row), static_cast<uint32_t>(column));
    return returnValue;
}

PyObject* PythonExtensions::OT_GetPortData(PyObject* _self, PyObject* _args) {
    auto numberOfArguments = PyTuple_Size(_args);
    const int expectedNumberOfArguments = 1;
    if (numberOfArguments != expectedNumberOfArguments) {
        throw std::exception("OT_GetPortData expects one arguments");
    }
    PythonObjectBuilder pyObBuilder;
    std::string portName = pyObBuilder.getStringValueFromTuple(_args, 0, "Parameter 0");
    PyObject* returnValue = PortDataBuffer::instance().getPortData(portName);
    return returnValue;
}

PyObject* PythonExtensions::OT_SetPortData(PyObject* _self, PyObject* _args) {
    auto numberOfArguments = PyTuple_Size(_args);
    const int expectedNumberOfArguments = 2;
    if (numberOfArguments != expectedNumberOfArguments) {
        throw std::exception("OT_GetPortData expects two argument");
    }
    PythonObjectBuilder pyObBuilder;
    std::string portName = pyObBuilder.getStringValueFromTuple(_args, 0, "Parameter 0");
    CPythonObjectBorrowed pvalue = pyObBuilder.getTupleItem(_args, 1, "Parameter 1");
    auto values = pyObBuilder.getGenericDataStructList(pvalue);
    PortDataBuffer::instance().overridePortData(portName, std::move(values));
    return PyBool_FromLong(true);
}
