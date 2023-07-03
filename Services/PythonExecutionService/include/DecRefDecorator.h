/*****************************************************************//**
 * \file   DecRefDecorator.h
 * \brief   General wrapper around a PyObject.
 *          This decrements the reference count on destruction.
 * 
 * \author Wagner
 * \date   May 2023
 *********************************************************************/
#pragma once
#include <Python.h>

class DecRefDecorator {
public:
    DecRefDecorator(PyObject* ref) : _ref{ ref } {}
    Py_ssize_t getRefCount() const { return _ref != nullptr ? Py_REFCNT(_ref) : 0; }
    
    DecRefDecorator& operator=(const DecRefDecorator& other) = delete;
    DecRefDecorator(const DecRefDecorator& other) = delete;


    // Allow setting of the (optional) argument with PyArg_ParseTupleAndKeywords
    //PyObject** operator&() {
    //    Py_XDECREF(_ref);
    //    _ref = nullptr;
    //    return &_ref;
    //}

    // Access the argument
    operator PyObject* () const { return _ref; }
    // Test if constructed successfully from the new reference.
    bool ReferenceIsSet() { return _ref != nullptr; }
    void FreePythonObject() 
    {
        Py_XDECREF(_ref); 
        _ref = nullptr;
    };

    ~DecRefDecorator() 
    { 
        Py_XDECREF(_ref); 
    }
protected:
    PyObject* _ref;
};