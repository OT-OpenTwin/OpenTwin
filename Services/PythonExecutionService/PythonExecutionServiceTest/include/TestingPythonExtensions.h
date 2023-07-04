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



    static PyMethodDef OTMethods[] = {

        {"WithOneParameter",  WithOneParameter, METH_VARARGS, "Test function with one parameter."},
        {"WithoutParameter",  WithoutParameter, METH_NOARGS, "Test function without parameter."},

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
