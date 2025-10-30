// @otlicense
// File: CPythonObject.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once
#include <Python.h>

//! @brief General wrapper around a PyObject.
//! This decrements the reference count on destruction.
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