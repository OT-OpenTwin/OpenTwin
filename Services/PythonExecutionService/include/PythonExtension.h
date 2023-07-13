#pragma once
#include <Python.h>
#include "PythonObjectBuilder.h"
#include "EntityBuffer.h"

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
            throw std::exception("OT_SetPropertyValue expects zero arguments");
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
            throw std::exception("OT_SetPropertyValue expects one argument");
        }
        EntityBuffer::INSTANCE().SaveChangedEntities();
        PythonObjectBuilder pyObBuilder;
        return pyObBuilder.setBool(true);
    }
    static PyMethodDef OTMethods[] = {

        {"GetPropertyValue",  OT_GetPropertyValue, METH_VARARGS, "Get the value of a requested property from a requested entity."},
        {"SetPropertyValue",  OT_SetPropertyValue, METH_VARARGS, "Set the property value of a requested property from a requested entity."},
        {"Flush",  OT_Flush, METH_NOARGS, "Apply all changes on entity properties."},
        {"FlushEntity",  OT_FlushEntity, METH_VARARGS, "Apply all changes on requested entity."},
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
