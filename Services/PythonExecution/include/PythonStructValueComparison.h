#pragma once
#include <Python.h>
#include "OTCore/QueryDescription/ValueComparisonDescription.h"

typedef struct {
    PyObject_HEAD
        PyObject* m_name;
    PyObject* m_comparator;
    PyObject* m_value;
} PythonStructValueComparison;

extern PyTypeObject PythonStructValueComparisonType;

bool       PythonStructValueComparison_registerType(PyObject* module);
PyObject* PythonStructValueComparison_create(const ot::ValueComparisonDescription& _valueComparison);
