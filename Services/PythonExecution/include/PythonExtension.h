/*****************************************************************//**
 * \file   PythonExtension.h
 * \brief  C Python Extension that hold functions for OpenTwin that can be called from a python script.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#pragma once
#include <Python.h>
#include "PythonObjectBuilder.h"
#include "EntityBuffer.h"
#include "PortDataBuffer.h"
#include "EntityFile.h"
#include "PythonModuleAPI.h"
#include "PythonLoadedModules.h"

namespace PythonExtensions
{
    static PyObject* OT_GetPropertyValue(PyObject* self, PyObject* args)
    {
         auto numberOfArguments = PyTuple_Size(args);
         const int expectedNumberOfArguments = 2;
         if (numberOfArguments != expectedNumberOfArguments)
         {
             throw std::exception("OT_GetPropertyValue expects two arguments");
         }
         PythonObjectBuilder pyObBuilder;
         std::string absoluteEntityName = pyObBuilder.getStringValueFromTuple(args,0,"Parameter 0");
         std::string propertyName = pyObBuilder.getStringValueFromTuple(args, 1, "Parameter 1");

         PyObject* returnValue = EntityBuffer::INSTANCE().GetEntityPropertyValue(absoluteEntityName, propertyName);
         return returnValue;
    }

    static PyObject* OT_GetScript(PyObject* self, PyObject* args)
    {
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 1;
        if (numberOfArguments != expectedNumberOfArguments)
        {
            throw std::exception("OT_GetScript expects one argument");
        }
        PythonObjectBuilder pyObBuilder;
        std::string absoluteScriptName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");
        

        auto baseEntity = EntityBuffer::INSTANCE().GetEntity(absoluteScriptName);
        ot::EntityInformation entityInfo(baseEntity.get());
        std::optional<std::string> moduleName =  PythonLoadedModules::INSTANCE()->getModuleName(entityInfo);
        CPythonObjectNew moduleImported(nullptr);

        if (!moduleName.has_value())
        {
            EntityFile* script = dynamic_cast<EntityFile*>(baseEntity.get());
            if (script == nullptr)
            {
                throw std::exception("Requested script execution cannot be done, since the entity is not a python script.");
            }

            PythonLoadedModules::INSTANCE()->AddModuleForEntity(entityInfo);
            moduleName = PythonLoadedModules::INSTANCE()->getModuleName(entityInfo);

            auto plainData = script->getData()->getData();
            std::string execution(plainData.begin(), plainData.end());
            CPythonObjectBorrowed  newModule(PyImport_AddModule(moduleName.value().c_str()));
            moduleImported = PyImport_ImportModule(moduleName.value().c_str());

            CPythonObjectBorrowed globalDictionary(PyModule_GetDict(moduleImported));
            CPythonObjectNew result(PyRun_String(execution.c_str(), Py_file_input, globalDictionary, globalDictionary));
            if (result == nullptr)
            {
                throw PythonException();
            }
            
        }
        else
        {
            moduleImported = PyImport_ImportModule(moduleName.value().c_str());
        }

        std::string entryPoint = PythonModuleAPI::GetModuleEntryPoint(moduleImported);
        return PyObject_GetAttrString(moduleImported, entryPoint.c_str());   
    }

    static PyObject* OT_SetPropertyValue(PyObject* self, PyObject* args)
    {
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 3;
        if (numberOfArguments != expectedNumberOfArguments)
        {
            throw std::exception("OT_SetPropertyValue expects three arguments");
        }

        PythonObjectBuilder pyObBuilder;
        std::string absoluteEntityName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");
        std::string propertyName = pyObBuilder.getStringValueFromTuple(args, 1, "Parameter 1");
        CPythonObjectBorrowed pvalue = pyObBuilder.getTupleItem(args, 2, "Parameter 2");

        EntityBuffer::INSTANCE().UpdateEntityPropertyValue(absoluteEntityName, propertyName,pvalue);
        return PyBool_FromLong(true);
    }

    static PyObject* OT_Flush(PyObject* self, PyObject* args)
    {
        if (args != nullptr)
        {
            throw std::exception("OT_Flush expects zero arguments");
        }

        EntityBuffer::INSTANCE().SaveChangedEntities();
        PythonObjectBuilder pyObBuilder;
        return PyBool_FromLong(true);
    }
    
    static PyObject* OT_FlushEntity(PyObject* self, PyObject* args)
    {
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 1;
        if (numberOfArguments != expectedNumberOfArguments)
        {
            throw std::exception("OT_FlushEntity expects one argument");
        }
        EntityBuffer::INSTANCE().SaveChangedEntities();
        PythonObjectBuilder pyObBuilder;
        return pyObBuilder.setBool(true);
    }

    static PyObject* OT_GetTableCell(PyObject* self, PyObject* args)
    {
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 3;
        if (numberOfArguments != expectedNumberOfArguments)
        {
            throw std::exception("OT_GetTableCell expects three arguments");
        }
        PythonObjectBuilder pyObBuilder;
        std::string absoluteEntityName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");
        int32_t row = pyObBuilder.getInt32ValueFromTuple(args, 1, "Parameter 1");
        int32_t column = pyObBuilder.getInt32ValueFromTuple(args, 2, "Parameter 2");

        PyObject* returnValue = EntityBuffer::INSTANCE().GetTableCellValue(absoluteEntityName, row, column);
        return returnValue;
    }
    
    static PyObject* OT_GetPortData(PyObject* self, PyObject* args)
    {
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 1;
        if (numberOfArguments != expectedNumberOfArguments)
        {
            throw std::exception("OT_GetPortData expects one arguments");
        }
        PythonObjectBuilder pyObBuilder;
        std::string portName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");
        PyObject* returnValue = PortDataBuffer::INSTANCE().getPortData(portName);
        return returnValue;
    }

    static PyObject* OT_SetPortData(PyObject* self, PyObject* args)
    {
        auto numberOfArguments = PyTuple_Size(args);
        const int expectedNumberOfArguments = 2;
        if (numberOfArguments != expectedNumberOfArguments)
        {
            throw std::exception("OT_GetPortData expects two argument");
        }
        PythonObjectBuilder pyObBuilder;
        std::string portName = pyObBuilder.getStringValueFromTuple(args, 0, "Parameter 0");
        CPythonObjectBorrowed pvalue = pyObBuilder.getTupleItem(args, 1, "Parameter 1");
        auto values = pyObBuilder.getGenericDataStructList(pvalue);
        PortDataBuffer::INSTANCE().OverridePortData(portName, std::move(values));
        return PyBool_FromLong(true);
    }

    static PyMethodDef OTMethods[] = {

        {"GetPropertyValue",  OT_GetPropertyValue, METH_VARARGS, "Get the value of a requested property from a requested entity."},
        {"GetTableCellValue",  OT_GetTableCell, METH_VARARGS, "Get the value of a cell from a requested entity."},
        {"SetPropertyValue",  OT_SetPropertyValue, METH_VARARGS, "Set the property value of a requested property from a requested entity."},
        {"Flush",  OT_Flush, METH_NOARGS, "Apply all changes on entity properties."},
        {"FlushEntity",  OT_FlushEntity, METH_VARARGS, "Apply all changes on requested entity."},
        {"GetPythonScript",  OT_GetScript, METH_VARARGS, "Executes another python script that is stored within OpenTwin."},
        {"GetPortData",  OT_GetPortData, METH_VARARGS, "For Block Items. Gets datachunks from the ingoing ports."},
        {"SetPortData",  OT_SetPortData, METH_VARARGS, "For Block Items. Sets datachunks from the outgoing ports."},
        {NULL, NULL, 0, NULL}        /* Sentinel */
    };

    static std::string otModuleDescription = "This module holds functions that call c++ functions, provided by the OpenTwin framework. These functions enable the access to OpenTwin's entities and their properties.";
    
    static struct PyModuleDef OTModule = {
        PyModuleDef_HEAD_INIT,
        "OpenTwin",   /* name of module */
        otModuleDescription.c_str(), /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                     or -1 if the module keeps state in global variables. */
        OTMethods
    };



    PyMODINIT_FUNC
        PyInit_OpenTwin(void)
    {
        return PyModule_Create(&OTModule);
    }

};
