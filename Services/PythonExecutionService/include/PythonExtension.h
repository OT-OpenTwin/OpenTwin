#pragma once
#include <Python.h>
#include "PythonObjectBuilder.h"

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
         std::string absoluteEntityName = PythonObjectBuilder::INSTANCE()->getStringValue(PyTuple_GetItem(args, 0), "Parameter 1");

         return new PyObject();
    }

    static PyMethodDef OTMethods[] = {

        {"OT_GetPropertyValue",  OT_GetPropertyValue, METH_VARARGS, "Execute a shell command."},

        {NULL, NULL, 0, NULL}        /* Sentinel */
    };

    static struct PyModuleDef OTModule = {
        PyModuleDef_HEAD_INIT,
        "OpenTwin",   /* name of module */
        NULL, /* module documentation, may be NULL */
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
