#pragma once
#include <Python.h>
#include "PythonObjectBuilder.h"

namespace PythonExtensions
{

    static PyObject* OT_Test(PyObject* self, PyObject* args)
    {
        int command = PythonObjectBuilder::INSTANCE()->getInt32Value(args, "Argument");
        
        if (!PyArg_ParseTuple(args, "i", &command))
        {
            return NULL;
        }
        return PyLong_FromLong(command + 13);
    }

    static PyMethodDef OTMethods[] = {

        {"OT_Test",  OT_Test, METH_VARARGS, "Execute a shell command."},

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
