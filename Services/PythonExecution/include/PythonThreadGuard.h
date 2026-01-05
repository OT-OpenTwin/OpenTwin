#pragma once
#include <Python.h>

class PythonThreadGuard
{
	public:
	PythonThreadGuard()
	{
		m_state = PyGILState_Ensure();
	}
	~PythonThreadGuard()
	{
		PyGILState_Release(m_state);
	}
private:
	PyGILState_STATE m_state;
};
