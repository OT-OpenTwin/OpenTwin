#pragma once
#define PY_SSIZE_T_CLEAN 
#include <Python.h>
#include <string>
#include <stdint.h>
#include <type_traits>

namespace PythonAPI
{

	PyObject* GetPythonVariableFromDictionary(PyObject* dict, const std::string& parameterName);


	template <class T>
	class PythonParameter
	{
	public:
		PythonParameter(const std::string& parameterName, const T& value);


		std::string getParameterString() const
		{
			return _parameterAsString;
		}

		//void getValueFromDictionary(PyObject* dictionary)
		//{
		//	_value = value;
		//	_parameterAsString = _parameterName + "=" + std::to_string(value);
		//}
		//T getValue() const { return _value; }

	private:
		std::string _parameterName;
		std::string _parameterAsString;
		T(*GetValueFromPythonDictionary)(PyObject* dict, const std::string& parameterName);
		T _value;
		//PyDict_SetItemString(localDictionary, "localOne", PyLong_FromLong(1));
		//long result = PyLong_AsLong(PyDict_GetItemString(localDictionary, "result"));
	};

	template<class T>
	inline PythonParameter<T>::PythonParameter(const std::string& parameterName, const T& value)
	{
		throw std::exception("Not supported type");
	}

	template<>
	inline PythonParameter<int32_t>::PythonParameter(const std::string& parameterName, const int32_t& value)
		: _parameterName(parameterName), _value(value)
	{
		_parameterAsString = _parameterName + "=" + std::to_string(value);
		GetValueFromPythonDictionary = [](PyObject* dict, const std::string& parameterName)
		{
			PyObject* var = PyDict_GetItemString(dict, parameterName.c_str());
			if (var == nullptr)
			{
				throw std::exception(("Failed to extract python variable " + parameterName + " from dictionary.").c_str());
			}
			if (PyLong_Check(var) == 1)
			{
				return static_cast<int32_t>(PyLong_AsLong(var));
			}
			else
			{
				throw std::exception(("Requested python parameter " + parameterName + " is not an integer type.").c_str());
			}
		};
	}

	template<>
	inline PythonParameter<int64_t>::PythonParameter(const std::string& parameterName, const int64_t& value)
		: _parameterName(parameterName), _value(value)
	{
		_parameterAsString = _parameterName + "=" + std::to_string(value);
		GetValueFromPythonDictionary = [](PyObject* dict, const std::string& parameterName)
		{
			PyObject* var = PyDict_GetItemString(dict, parameterName.c_str());
			if (var == nullptr)
			{
				throw std::exception(("Failed to extract python variable " + parameterName + " from dictionary.").c_str());
			}
			if (PyLong_Check(var) == 1)
			{
				return static_cast<int64_t>(PyLong_AsLongLong(var));
			}
			else
			{
				throw std::exception(("Requested python parameter " + parameterName + " is not an integer type.").c_str());
			}
		};
	}

	template<>
	inline PythonParameter<double>::PythonParameter(const std::string& parameterName, const double& value)
		: _parameterName(parameterName), _value(value)
	{
		_parameterAsString = _parameterName + "=" + std::to_string(value);
		GetValueFromPythonDictionary = [](PyObject* dict, const std::string& parameterName)
		{
			PyObject* var = PyDict_GetItemString(dict, parameterName.c_str());
			if (var == nullptr)
			{
				throw std::exception(("Failed to extract python variable " + parameterName + " from dictionary.").c_str());
			}
			if (PyFloat_Check(var) == 1)
			{
				return (PyFloat_AsDouble(var));
			}
			else
			{
				throw std::exception(("Requested python parameter " + parameterName + " is not an floating type.").c_str());
			}
		};
	}

	template<>
	inline PythonParameter<float>::PythonParameter(const std::string& parameterName, const float& value)
		: _parameterName(parameterName), _value(value)
	{
		_parameterAsString = _parameterName + "=" + std::to_string(value);
		GetValueFromPythonDictionary = [](PyObject* dict, const std::string& parameterName)
		{
			PyObject* var = PyDict_GetItemString(dict, parameterName.c_str());
			if (var == nullptr)
			{
				throw std::exception(("Failed to extract python variable " + parameterName + " from dictionary.").c_str());
			}
			if (PyFloat_Check(var) == 1)
			{
				return (static_cast<float>(PyFloat_AsDouble(var)));
			}
			else
			{
				throw std::exception(("Requested python parameter " + parameterName + " is not an floating type.").c_str());
			}
		};
	}

	template<>
	inline PythonParameter<std::string>::PythonParameter(const std::string& parameterName, const std::string& value)
		: _parameterName(parameterName), _value(value)
	{
		_parameterAsString = _parameterName + "=" + value;
		GetValueFromPythonDictionary = [](PyObject* dict, const std::string& parameterName)
		{
			PyObject* var = PyDict_GetItemString(dict, parameterName.c_str());
			if (var == nullptr)
			{
				throw std::exception(("Failed to extract python variable " + parameterName + " from dictionary.").c_str());
			}
			if (PyUnicode_Check(var) == 1)
			{
				return std::string(PyUnicode_AsUTF8(var));
			}
			else
			{
				throw std::exception(("Requested python parameter " + parameterName + " is not an unicode type.").c_str());
			}
		};
	}

	template<>
	inline PythonParameter<bool>::PythonParameter(const std::string& parameterName, const bool& value)
		: _parameterName(parameterName), _value(value)
	{
		_parameterAsString = _parameterName + "=" + std::to_string(value);
		GetValueFromPythonDictionary = [](PyObject* dict, const std::string& parameterName)
		{
			PyObject* var = PyDict_GetItemString(dict, parameterName.c_str());
			if (var == nullptr)
			{
				throw std::exception(("Failed to extract python variable " + parameterName + " from dictionary.").c_str());
			}
			if (PyBool_Check(var) == 1)
			{
				long value = PyLong_AsLong(var);
				return value == 1;
			}
			else
			{
				throw std::exception(("Requested python parameter " + parameterName + " is not an unicode type.").c_str());
			}
		};
	}
}