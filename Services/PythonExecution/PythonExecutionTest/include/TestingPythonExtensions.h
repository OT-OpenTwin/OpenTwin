// @otlicense

#pragma once
#pragma once
#include <Python.h>
#include "PythonObjectBuilder.h"

namespace TestingPythonExtensions
{
 
    static PyObject* GetFunction(PyObject* self, PyObject* args)
    {
        PythonObjectBuilder pyObBuilder;
        std::string scriptName = pyObBuilder.getStringValue(PyTuple_GetItem(args, 0), "Parameter 1");
        std::string functionName = pyObBuilder.getStringValue(PyTuple_GetItem(args, 1), "Parameter 2");
        PyObject* module = PyImport_ImportModule(scriptName.c_str());
        assert(module != nullptr);
        PyObject* function = PyObject_GetAttrString(module, functionName.c_str());
        assert(function != nullptr);
        return function;
    }

    static PyObject* WithOneParameter(PyObject* self, PyObject* args)
    {
        int command;
        if (!PyArg_ParseTuple(args, "i", &command))
        {
            return NULL;
        }
        return PyLong_FromLong(command + 13);
    }

    static PyObject* WithoutParameter(PyObject* self, PyObject* args)
    {
        return PyUnicode_FromString("Hello from the extension");
    }

    static PyObject* WithMultipleParameter(PyObject* self, PyObject* args)
    {
        PythonObjectBuilder pyObBuilder;
        CPythonObjectBorrowed param1(PyTuple_GetItem(args, 0));
        int parameter1 = pyObBuilder.getInt32Value(param1, "Parameter 1");
        CPythonObjectBorrowed param2(PyTuple_GetItem(args, 1));
        int parameter2 = pyObBuilder.getInt32Value(param2,"Parameter 2");
        CPythonObjectBorrowed param3(PyTuple_GetItem(args, 2));
        std::string parameter3 = pyObBuilder.getStringValue(param3,"Parameter 3");

        int result = (parameter1 + parameter2) * 2;
        if (parameter3 == "Suppe")
        {
            result += 7;
        }
        
        return pyObBuilder.setInt32(result);
    }

    static PyMethodDef OTMethods[] = {

        {"WithOneParameter",  WithOneParameter, METH_VARARGS, "Test function with one parameter."},
        {"WithoutParameter",  WithoutParameter, METH_NOARGS, "Test function without parameter."},
        {"WithMultipleParameter",  WithMultipleParameter, METH_VARARGS, "Test function with multiple parameter."},
        {"GetFunction",  GetFunction, METH_VARARGS, "Test function getter."},

        {NULL, NULL, 0, NULL}        /* Sentinel */
    };

    static std::string testModuleDescription = "This module holds functions that are used for unit tests.";

    static struct PyModuleDef TestModule = {
        PyModuleDef_HEAD_INIT,
        "InitialTestModule",   /* name of module */
        testModuleDescription.c_str(), /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                     or -1 if the module keeps state in global variables. */
        OTMethods
    };

    PyMODINIT_FUNC
        PyInit_Testing(void)
    {
        return PyModule_Create(&TestModule);
    }

};
