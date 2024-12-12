/*****************************************************************//**
 * \file   PythonExtension.h
 * \brief  C Python Extension that hold functions for OpenTwin that can be called from a python script.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#pragma once

#include <Python.h>

#include <string>

namespace PythonExtensions {

    static PyObject* OT_GetPropertyValue(PyObject* _self, PyObject* _args);

    static PyObject* OT_GetScript(PyObject* _self, PyObject* _args);

    static PyObject* OT_SetPropertyValue(PyObject* _self, PyObject* _args);

    static PyObject* OT_Flush(PyObject* _self, PyObject* _args);
    
    static PyObject* OT_FlushEntity(PyObject* _self, PyObject* _args);

    static PyObject* OT_GetTableCell(PyObject* _self, PyObject* _args);
    
    static PyObject* OT_GetPortData(PyObject* _self, PyObject* _args);

    static PyObject* OT_SetPortData(PyObject* _self, PyObject* _args);

    static PyMethodDef OTMethods[] = {

        {"GetPropertyValue", OT_GetPropertyValue, METH_VARARGS, "Get the value of a requested property from a requested entity."},
        {"GetTableCellValue", OT_GetTableCell, METH_VARARGS, "Get the value of a cell from a requested entity."},
        {"SetPropertyValue", OT_SetPropertyValue, METH_VARARGS, "Set the property value of a requested property from a requested entity."},
        {"Flush", OT_Flush, METH_NOARGS, "Apply all changes on entity properties."},
        {"FlushEntity", OT_FlushEntity, METH_VARARGS, "Apply all changes on requested entity."},
        {"GetPythonScript", OT_GetScript, METH_VARARGS, "Executes another python script that is stored within OpenTwin."},
        {"GetPortData", OT_GetPortData, METH_VARARGS, "For Block Items. Gets datachunks from the ingoing ports."},
        {"SetPortData", OT_SetPortData, METH_VARARGS, "For Block Items. Sets datachunks from the outgoing ports."},
        {NULL, NULL, 0, NULL}        // Sentinel
    };

    static std::string otModuleDescription = "This module holds functions that call c++ functions, provided by the OpenTwin framework. These functions enable the access to OpenTwin's entities and their properties.";
    
    static struct PyModuleDef OTModule = {
        PyModuleDef_HEAD_INIT,
        "OpenTwin",                  // name of module
        otModuleDescription.c_str(), // module documentation, may be NULL
        -1,                          // size of per-interpreter state of the module, or -1 if the module keeps state in global variables.
        OTMethods
    };



    PyMODINIT_FUNC
        PyInit_OpenTwin(void)
    {
        return PyModule_Create(&OTModule);
    }

};

#include "PythonExtension.hpp"