/*****************************************************************//**
 * \file   CPythonObjectNew.h
 * \brief	Wrapper around a PyObject* that is a new reference.
 *			This owns the reference so does not increment it on construction but 
 *			does decrement it on destruction.
 * 
 * \author Wagner
 * \date   May 2023
 *********************************************************************/





#pragma once
#include "CPythonObject.h"

class CPythonObjectNew : public CPythonObject
{	
public:
	CPythonObjectNew(PyObject* newRef) : CPythonObject(newRef) {}

    using CPythonObject::reset;

    void reset(PyObject* ref)
    {
        if (_ref != nullptr)
        {
            Py_XDECREF(_ref);
            _ref = nullptr;
        }
        _ref = ref;
    }


};
