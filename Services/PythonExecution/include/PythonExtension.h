// @otlicense
// File: PythonExtension.h
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

#include <Python.h>

#include <string>

//! @brief C Python Extension that hold functions for OpenTwin that can be called from a python script.
//! Neither of the function parameter's memory is owned. They have to be turned into CPythonObjectBorrowed within the function implementation.
namespace PythonExtensions {

    static PyObject* OT_GetPropertyValue(PyObject* _self, PyObject* _args);

    static PyObject* OT_GetScript(PyObject* _self, PyObject* _args);

    static PyObject* OT_SetPropertyValue(PyObject* _self, PyObject* _args);

    static PyObject* OT_Flush(PyObject* _self, PyObject* _args);
    
    static PyObject* OT_FlushEntity(PyObject* _self, PyObject* _args);

    static PyObject* OT_GetTableCell(PyObject* _self, PyObject* _args);
    
    static PyObject* OT_GetPortData(PyObject* _self, PyObject* _args);
    
    static PyObject* OT_GetPortMetaData(PyObject* _self, PyObject* _args);

    static PyObject* OT_SetPortData(PyObject* _self, PyObject* _args);
    
    static PyObject* OT_SetPortMetaData(PyObject* _self, PyObject* _args);

    static PyMethodDef OTMethods[] = {

        {"GetPropertyValue", OT_GetPropertyValue, METH_VARARGS, "Get the value of a requested property from a requested entity."},
        {"GetTableCellValue", OT_GetTableCell, METH_VARARGS, "Get the value of a cell from a requested entity."},
        {"SetPropertyValue", OT_SetPropertyValue, METH_VARARGS, "Set the property value of a requested property from a requested entity."},
        {"Flush", OT_Flush, METH_NOARGS, "Apply all changes on entity properties."},
        {"FlushEntity", OT_FlushEntity, METH_VARARGS, "Apply all changes on requested entity."},
        {"GetPythonScript", OT_GetScript, METH_VARARGS, "Executes another python script that is stored within OpenTwin."},
        {"GetPortData", OT_GetPortData, METH_VARARGS, "For Block Items. Gets datachunks from the ingoing ports."},
        {"GetPortMetaData", OT_GetPortMetaData, METH_VARARGS, "For Block Items. Gets meta data from the ingoing ports."},
        {"SetPortData", OT_SetPortData, METH_VARARGS, "For Block Items. Sets datachunks from the outgoing ports."},
        {"SetPortMetaData", OT_SetPortMetaData, METH_VARARGS, "For Block Items. Sets metadata of the outgoing ports."},
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