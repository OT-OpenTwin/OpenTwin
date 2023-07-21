#include "PythonModuleAPI.h"
#include "PythonObjectBuilder.h"

std::string PythonModuleAPI::GetModuleEntryPoint(const std::string& moduleName)
{
	PythonObjectBuilder pythonObjectBuilder;
	CPythonObjectBorrowed module(PyImport_ImportModule(moduleName.c_str()));
	CPythonObjectBorrowed globalDictionary = PyModule_GetDict(module);
	CPythonObjectBorrowed allGlobalDictItems = pythonObjectBuilder.getDictItem(globalDictionary);

	bool firstOfFunctionNames = false;
	std::string entryPointName = "";

	for (int i = 0; i < PyList_Size(allGlobalDictItems); i++)
	{
		CPythonObjectBorrowed listEntry = pythonObjectBuilder.getListItem(allGlobalDictItems, i);
		std::string listEntryName = pythonObjectBuilder.getStringValueFromTuple(listEntry, 0, "ListValue");


		if (listEntryName == "__builtins__") //Last default entry
		{
			firstOfFunctionNames = true;
		}
		else if (firstOfFunctionNames)
		{
			CPythonObjectBorrowed listElement(PyDict_GetItemString(globalDictionary, listEntryName.c_str()));

			if (PyFunction_Check(listElement))
			{
				if (entryPointName == "")
				{
					entryPointName = listEntryName;
				}
				else
				{
					entryPointName = "__main__";
					break;
				}
			}
		}
	}

	return entryPointName;
}
