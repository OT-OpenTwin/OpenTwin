#pragma once
// @otlicense
#include <Python.h>
#include <map>
#include <string>


#pragma once
#include <Python.h>

typedef struct {
    PyObject_HEAD
    PyObject* m_name;
    PyObject* m_entityType;
    PyObject* m_data;
} EntityBuilder;

extern PyTypeObject EntityBuilderType;

bool      EntityBuilder_registerType(PyObject* module);



// Example
