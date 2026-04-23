#pragma once
#include <Python.h>




// This is the Python object in memory
typedef struct {
    PyObject_HEAD          // Mandatory — ref count + type pointer (no semicolon!)
    PyObject* name;        // Python str object
    double    value;       // Plain C double — stored directly
    int       count;       // Plain C int
} TestStruct;

// Strings must be heap-allocated Python objects so the GC can track them. Scalars (int, double, float) can be stored as raw C types directly in the struct — Python wraps them on read via PyMemberDef

// ── Type object — defined in TestStruct.cpp, declared here for registration ─
extern PyTypeObject TestStructType;
// ── Factory — declared here so OT_* functions in other .cpp files can call it
bool TestStruct_registerType(PyObject* module);
// Factory to create such an object
PyObject* TestStruct_create(const char* name, double value, int count);

