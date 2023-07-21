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

class CPythonObject{
public:
    CPythonObject(PyObject* ref) : _ref{ ref } {}
    Py_ssize_t getRefCount() const { return _ref != nullptr ? Py_REFCNT(_ref) : 0; }
    
    CPythonObject& operator=(const CPythonObject& other) = delete;
    CPythonObject(const CPythonObject& other) = delete;

    CPythonObject& operator=(CPythonObject&& other) noexcept
    {
        this->_ref = other._ref;
        other._ref = nullptr;
        return *this;
    };

    CPythonObject(CPythonObject&& other) noexcept
    {
        this->_ref = other._ref;
        other._ref = nullptr;
    }

    void reset(CPythonObject&& other)
    {
        if (_ref != nullptr)
        {
            Py_XDECREF(_ref);
            _ref = nullptr;
        }
        _ref = other._ref;
        other._ref = nullptr;
    }

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

    void DropOwnership() { _ref = nullptr; }

    void FreePythonObject() 
    {
        Py_XDECREF(_ref); 
        _ref = nullptr;
    };

    ~CPythonObject()
    { 
        Py_XDECREF(_ref); 
    }
protected:
    PyObject* _ref;
};