// @otlicense

/*****************************************************************//**
 * \file   CPythonObjectBorrowed.h
 * \brief   Wrapper around a PyObject* that is a borrowed reference.
 *          This increments the reference count on construction and
 *          decrements the reference count on destruction.
 * 
 * \author Wagner
 * \date   May 2023
 *********************************************************************/

#pragma once
#include "CPythonObject.h"

class CPythonObjectBorrowed : public CPythonObject
{
public:
    CPythonObjectBorrowed(PyObject* borrowedRef) : CPythonObject(borrowedRef)
    {
        Py_XINCREF(_ref);
    }
    
    using CPythonObject::reset;

    void reset(PyObject* ref)
    {
        if (_ref != nullptr)
        {
            Py_XDECREF(_ref);
            _ref = nullptr;
        }
        _ref = ref;
        Py_XINCREF(_ref);
    }
};
