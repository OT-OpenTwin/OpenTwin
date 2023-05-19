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
#include "DecRefDecorator.h"
class CPythonObjectBorrowed : public DecRefDecorator
{
public:
    CPythonObjectBorrowed(PyObject* borrowedRef) : DecRefDecorator(borrowedRef) 
    {
        Py_XINCREF(_ref);
    }
};
