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
#include "DecRefDecorator.h"

class CPythonObjectNew : public DecRefDecorator
{	
public:
	CPythonObjectNew(PyObject* newRef) : DecRefDecorator(newRef) {}
};
