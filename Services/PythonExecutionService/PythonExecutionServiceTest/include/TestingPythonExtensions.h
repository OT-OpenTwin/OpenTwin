#pragma once
#pragma once
#include <Python.h>
#include "PythonObjectBuilder.h"

namespace TestingPythonExtensions
{

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

        int parameter1 = PythonObjectBuilder::INSTANCE()->getInt32Value(PyTuple_GetItem(args, 0),"Parameter 1");
        int parameter2 = PythonObjectBuilder::INSTANCE()->getInt32Value(PyTuple_GetItem(args, 1),"Parameter 2");
        std::string parameter3 = PythonObjectBuilder::INSTANCE()->getStringValue(PyTuple_GetItem(args, 2),"Parameter 3");

        int result = (parameter1 + parameter2) * 2;
        if (parameter3 == "Suppe")
        {
            result += 7;
        }
        
        return PythonObjectBuilder::INSTANCE()->setInt32(result);
    }

    static PyMethodDef OTMethods[] = {

        {"WithOneParameter",  WithOneParameter, METH_VARARGS, "Test function with one parameter."},
        {"WithoutParameter",  WithoutParameter, METH_NOARGS, "Test function without parameter."},
        {"WithMultipleParameter",  WithMultipleParameter, METH_VARARGS, "Test function with multiple parameter."},

        {NULL, NULL, 0, NULL}        /* Sentinel */
    };

    static struct PyModuleDef TestModule = {
        PyModuleDef_HEAD_INIT,
        "InitialTestModule",   /* name of module */
        NULL, /* module documentation, may be NULL */
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
