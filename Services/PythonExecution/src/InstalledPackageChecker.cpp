#include "InstalledPackageChecker.h"
#include <unordered_set>
#include <algorithm>
#include <sstream>
#include "PythonException.h"

bool InstalledPackageChecker::refreshPackageCaches()
{
    m_installedPackageVersionsByCanonicalName.clear();
    m_importToDistributionsByCanonicalImport.clear();

    PyObject* metadataMod = PyImport_ImportModule("importlib.metadata");
    PyObject* packagingUtilsMod = PyImport_ImportModule("packaging.utils");
    if (!metadataMod || !packagingUtilsMod)
    {
        Py_XDECREF(metadataMod);
        Py_XDECREF(packagingUtilsMod);
        throw PythonException();
        PyErr_Clear();
        return false;
    }

    // HEre we get the lookup from import name to package name
    PyObject* canonicalizeNameFunc = PyObject_GetAttrString(packagingUtilsMod, "canonicalize_name");
    //  iterates over installed distributions
    PyObject* distributionsFunc = PyObject_GetAttrString(metadataMod, "distributions");
    PyObject* packagesDistributionsFunc = PyObject_GetAttrString(metadataMod, "packages_distributions");

    Py_DECREF(packagingUtilsMod);
    Py_DECREF(metadataMod);

    if (!canonicalizeNameFunc || !distributionsFunc || !packagesDistributionsFunc)
    {
        Py_XDECREF(canonicalizeNameFunc);
        Py_XDECREF(distributionsFunc);
        Py_XDECREF(packagesDistributionsFunc);
        PyErr_Clear();
        return false;
    }

    // Here we iterate over all installed packages and get their key-information
    PyObject* distsIterObj = PyObject_CallObject(distributionsFunc, nullptr);
    if (!distsIterObj)
    {
        Py_DECREF(canonicalizeNameFunc);
        Py_DECREF(distributionsFunc);
        Py_DECREF(packagesDistributionsFunc);
        PyErr_Clear();
        return false;
    }

    PyObject* distsIter = PyObject_GetIter(distsIterObj);
    Py_DECREF(distsIterObj);

    if (!distsIter)
    {
        Py_DECREF(canonicalizeNameFunc);
        Py_DECREF(distributionsFunc);
        Py_DECREF(packagesDistributionsFunc);
        PyErr_Clear();
        return false;
    }

    for (;;)
    {
        PyObject* dist = PyIter_Next(distsIter);
        if (!dist)
            break;

        PyObject* metadataObj = PyObject_GetAttrString(dist, "metadata");
        PyObject* versionObj = PyObject_GetAttrString(dist, "version");

        if (metadataObj && versionObj)
        {
            PyObject* keyObj = PyUnicode_FromString("Name");
            PyObject* nameObj = keyObj ? PyObject_GetItem(metadataObj, keyObj) : nullptr;
            Py_XDECREF(keyObj);

            if (nameObj)
            {
                PyObject* canonArgs = PyTuple_Pack(1, nameObj);
                PyObject* canonNameObj = canonArgs ? PyObject_CallObject(canonicalizeNameFunc, canonArgs) : nullptr;
                Py_XDECREF(canonArgs);

                const char* canonicalName = canonNameObj ? PyUnicode_AsUTF8(canonNameObj) : nullptr;
                const char* version = PyUnicode_AsUTF8(versionObj);

                if (canonicalName && version)
                {
                    m_installedPackageVersionsByCanonicalName[canonicalName] = version;
                }

                Py_XDECREF(canonNameObj);
                Py_DECREF(nameObj);
            }
        }

        Py_XDECREF(metadataObj);
        Py_XDECREF(versionObj);
        Py_DECREF(dist);
    }

    Py_DECREF(distsIter);

    // Here we build up the package name loop-up.
    // It will contain entries like:
    // "numpy": ["numpy"] ,
    // "sklearn" : ["scikit-learn"] ,
    // "PIL" : ["Pillow"]
    PyObject* mappingObj = PyObject_CallObject(packagesDistributionsFunc, nullptr);
    if (mappingObj && PyDict_Check(mappingObj))
    {
        PyObject* key = nullptr;
        PyObject* value = nullptr;
        Py_ssize_t pos = 0;

        while (PyDict_Next(mappingObj, &pos, &key, &value))
        {
            const char* importName = PyUnicode_AsUTF8(key);
            if (!importName)
                continue;

            std::string normalizedImport = normalizeImportName(importName);
            auto distributions = pyListToStringVector(value);
            auto& out = m_importToDistributionsByCanonicalImport[normalizedImport];

            for (const auto& distName : distributions)
            {
                PyObject* distNameObj = PyUnicode_FromString(distName.c_str());
                if (!distNameObj)
                    continue;

                PyObject* canonArgs = PyTuple_Pack(1, distNameObj);
                Py_DECREF(distNameObj);

                PyObject* canonNameObj = canonArgs ? PyObject_CallObject(canonicalizeNameFunc, canonArgs) : nullptr;
                Py_XDECREF(canonArgs);

                const char* canonicalDist = canonNameObj ? PyUnicode_AsUTF8(canonNameObj) : nullptr;
                if (canonicalDist)
                    out.emplace_back(canonicalDist);

                Py_XDECREF(canonNameObj);
            }
        }
    }

    Py_XDECREF(mappingObj);
    Py_DECREF(canonicalizeNameFunc);
    Py_DECREF(distributionsFunc);
    Py_DECREF(packagesDistributionsFunc);

    return true;
}

bool InstalledPackageChecker::isRequirementSatisfied(const std::string& requirementLine) const
{
    const std::string line = trim(requirementLine);
    if (line.empty() || line[0] == '#')
        return true;

    // We are using the python class packaging.requirements.Requirement class to parse the import statements for its components
    PyObject* reqModule = PyImport_ImportModule("packaging.requirements");
    PyObject* utilsModule = PyImport_ImportModule("packaging.utils");
    if (!reqModule || !utilsModule)
    {
        Py_XDECREF(reqModule);
        Py_XDECREF(utilsModule);
        PyErr_Clear();
        return false;
    }

    PyObject* requirementClass = PyObject_GetAttrString(reqModule, "Requirement");
    PyObject* canonicalizeNameFunc = PyObject_GetAttrString(utilsModule, "canonicalize_name");

    Py_DECREF(reqModule);
    Py_DECREF(utilsModule);

    if (!requirementClass || !canonicalizeNameFunc)
    {
        Py_XDECREF(requirementClass);
        Py_XDECREF(canonicalizeNameFunc);
        PyErr_Clear();
        return false;
    }

    PyObject* lineObj = PyUnicode_FromString(line.c_str());
    if (!lineObj)
    {
        Py_DECREF(requirementClass);
        Py_DECREF(canonicalizeNameFunc);
        PyErr_Clear();
        return false;
    }

    PyObject* reqArgs = PyTuple_Pack(1, lineObj);
    Py_DECREF(lineObj);

    //Now we get the components of the requirement line
    PyObject* reqObj = reqArgs ? PyObject_CallObject(requirementClass, reqArgs) : nullptr;
    Py_XDECREF(reqArgs);
    Py_DECREF(requirementClass);

    if (!reqObj)
    {
        Py_DECREF(canonicalizeNameFunc);
        PyErr_Clear();
        return false;
    }

    PyObject* nameObj = PyObject_GetAttrString(reqObj, "name");
    PyObject* specifierObj = PyObject_GetAttrString(reqObj, "specifier");
    PyObject* urlObj = PyObject_GetAttrString(reqObj, "url");

    if (!nameObj || !specifierObj || !urlObj)
    {
        Py_XDECREF(nameObj);
        Py_XDECREF(specifierObj);
        Py_XDECREF(urlObj);
        Py_DECREF(reqObj);
        Py_DECREF(canonicalizeNameFunc);
        PyErr_Clear();
        return false;
    }

    PyObject* canonArgs = PyTuple_Pack(1, nameObj);
    PyObject* canonicalNameObj = canonArgs ? PyObject_CallObject(canonicalizeNameFunc, canonArgs) : nullptr;
    Py_XDECREF(canonArgs);
    Py_DECREF(canonicalizeNameFunc);

    const char* canonicalName = canonicalNameObj ? PyUnicode_AsUTF8(canonicalNameObj) : nullptr;
    if (!canonicalName)
    {
        Py_XDECREF(canonicalNameObj);
        Py_DECREF(nameObj);
        Py_DECREF(specifierObj);
        Py_DECREF(urlObj);
        Py_DECREF(reqObj);
        PyErr_Clear();
        return false;
    }
    
    //Finally we can check witch the cache if installed 
    auto installedIt = m_installedPackageVersionsByCanonicalName.find(canonicalName);
    const bool installed = installedIt != m_installedPackageVersionsByCanonicalName.end();

    if (!installed)
    {
        Py_DECREF(canonicalNameObj);
        Py_DECREF(nameObj);
        Py_DECREF(specifierObj);
        Py_DECREF(urlObj);
        Py_DECREF(reqObj);
        return false;
    }

    const char* urlText = (urlObj == Py_None) ? nullptr : PyUnicode_AsUTF8(urlObj);

    PyObject* specifierStrObj = PyObject_Str(specifierObj);
    const char* specifierText = specifierStrObj ? PyUnicode_AsUTF8(specifierStrObj) : nullptr;

    bool result = true;

    if (urlText && *urlText)
    {
        result = true;
    }
    else if (specifierText && *specifierText)
    {
        PyObject* containsFunc = PyObject_GetAttrString(specifierObj, "contains");
        if (!containsFunc)
        {
            result = false;
            PyErr_Clear();
        }
        else
        {
            PyObject* versionObj = PyUnicode_FromString(installedIt->second.c_str());
            PyObject* kwargs = PyDict_New();
            if (kwargs)
                PyDict_SetItemString(kwargs, "prereleases", Py_True);

            PyObject* containsArgs = versionObj ? PyTuple_Pack(1, versionObj) : nullptr;
            Py_XDECREF(versionObj);

            PyObject* containsResult = (containsArgs && kwargs)
                ? PyObject_Call(containsFunc, containsArgs, kwargs)
                : nullptr;

            Py_XDECREF(containsArgs);
            Py_XDECREF(kwargs);
            Py_DECREF(containsFunc);

            if (!containsResult)
            {
                result = false;
                PyErr_Clear();
            }
            else
            {
                result = PyObject_IsTrue(containsResult) == 1;
                Py_DECREF(containsResult);
            }
        }
    }

    Py_XDECREF(specifierStrObj);
    Py_DECREF(canonicalNameObj);
    Py_DECREF(nameObj);
    Py_DECREF(specifierObj);
    Py_DECREF(urlObj);
    Py_DECREF(reqObj);

    return result;
}

std::vector<std::string> InstalledPackageChecker::findMissingDistributionsForScriptImports(const std::string& scriptText) const
{
    //Transforms:
    // import numpy as np
    // from sklearn.model_selection import train_test_split
    // import requests
    // into:
    // std::vector<std::string> = { "numpy", "scikit-learn", "requests" }
   
    std::vector<std::string> missingDistributions;
    std::unordered_set<std::string> seen;
    // The ast - Abstract syntax trees package (https://docs.python.org/3/library/ast.html) helps to process the python abstract syntax grammar
    PyObject* astModule = PyImport_ImportModule("ast");
    if (!astModule)
    {
        PyErr_Clear();
        return missingDistributions;
    }

    PyObject* parseFunc = PyObject_GetAttrString(astModule, "parse");
    Py_DECREF(astModule);

    if (!parseFunc)
    {
        PyErr_Clear();
        return missingDistributions;
    }

    PyObject* scriptObj = PyUnicode_FromStringAndSize(scriptText.c_str(),
        static_cast<Py_ssize_t>(scriptText.size()));
    if (!scriptObj)
    {
        Py_DECREF(parseFunc);
        PyErr_Clear();
        return missingDistributions;
    }

    PyObject* parseArgs = PyTuple_Pack(1, scriptObj);
    Py_DECREF(scriptObj);

    PyObject* tree = parseArgs ? PyObject_CallObject(parseFunc, parseArgs) : nullptr;
    Py_XDECREF(parseArgs);
    Py_DECREF(parseFunc);

    if (!tree)
    {
        PyErr_Clear();
        return missingDistributions;
    }

    const char* extractorCode =
        "def _extract_top_level_imports(tree):\n"
        "    import ast\n"
        "    result = set()\n"
        "    for node in ast.walk(tree):\n"
        "        if isinstance(node, ast.Import):\n"
        "            for alias in node.names:\n"
        "                result.add(alias.name.split('.')[0])\n"
        "        elif isinstance(node, ast.ImportFrom):\n"
        "            if node.level == 0 and node.module:\n"
        "                result.add(node.module.split('.')[0])\n"
        "    return sorted(result)\n";

    PyObject* globals = PyDict_New();
    PyObject* locals = PyDict_New();

    if (!globals || !locals)
    {
        Py_XDECREF(globals);
        Py_XDECREF(locals);
        Py_DECREF(tree);
        PyErr_Clear();
        return missingDistributions;
    }

    PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());

    PyObject* execObj = PyRun_String(extractorCode, Py_file_input, globals, locals);
    Py_XDECREF(execObj);

    PyObject* extractorFunc = PyDict_GetItemString(locals, "_extract_top_level_imports");
    if (!extractorFunc)
    {
        Py_DECREF(globals);
        Py_DECREF(locals);
        Py_DECREF(tree);
        PyErr_Clear();
        return missingDistributions;
    }

    PyObject* extractorArgs = PyTuple_Pack(1, tree);
    Py_DECREF(tree);

    PyObject* importListObj = extractorArgs ? PyObject_CallObject(extractorFunc, extractorArgs) : nullptr;
    Py_XDECREF(extractorArgs);
    Py_DECREF(globals);
    Py_DECREF(locals);

    if (!importListObj)
    {
        PyErr_Clear();
        return missingDistributions;
    }

    auto importNames = pyListToStringVector(importListObj);
    Py_DECREF(importListObj);

    for (const auto& importName : importNames)
    {
        if (isImportableModule(importName))
            continue;

        const std::string normalizedImport = normalizeImportName(importName);
        auto it = m_importToDistributionsByCanonicalImport.find(normalizedImport);

        if (it != m_importToDistributionsByCanonicalImport.end() && !it->second.empty())
        {
            for (const auto& dist : it->second)
            {
                if (!isInstalledDistributionCanonical(dist) && seen.insert(dist).second)
                    missingDistributions.push_back(dist);
            }
        }
        else
        {
            if (seen.insert(normalizedImport).second)
                missingDistributions.push_back(normalizedImport);
        }
    }

    return missingDistributions;
}

std::string InstalledPackageChecker::buildRequirementsText() const
{
    std::vector<std::pair<std::string, std::string>> packages(
        m_installedPackageVersionsByCanonicalName.begin(),
        m_installedPackageVersionsByCanonicalName.end()
    );

    std::sort(packages.begin(), packages.end(),
        [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

    std::ostringstream out;
    for (const auto& [name, version] : packages)
    {
        out << name << "==" << version << "\n";
    }

    return out.str();
}

uint64_t InstalledPackageChecker::getNumberOfInstalledPackages()
{
    return m_installedPackageVersionsByCanonicalName.size();
}

bool InstalledPackageChecker::isInstalledDistributionCanonical(const std::string& _canonicalName) const
{
    return m_installedPackageVersionsByCanonicalName.find(_canonicalName) !=
        m_installedPackageVersionsByCanonicalName.end();
}

bool InstalledPackageChecker::isImportableModule(const std::string& _moduleName) const
{
    PyObject* importlibUtil = PyImport_ImportModule("importlib.util");
    if (!importlibUtil)
    {
        PyErr_Clear();
        return false;
    }

    PyObject* findSpec = PyObject_GetAttrString(importlibUtil, "find_spec");
    Py_DECREF(importlibUtil);

    if (!findSpec)
    {
        PyErr_Clear();
        return false;
    }

    PyObject* pyName = PyUnicode_FromString(_moduleName.c_str());
    if (!pyName)
    {
        Py_DECREF(findSpec);
        PyErr_Clear();
        return false;
    }

    PyObject* args = PyTuple_Pack(1, pyName);
    Py_DECREF(pyName);

    if (!args)
    {
        Py_DECREF(findSpec);
        PyErr_Clear();
        return false;
    }

    PyObject* spec = PyObject_CallObject(findSpec, args);
    Py_DECREF(args);
    Py_DECREF(findSpec);

    if (!spec)
    {
        PyErr_Clear();
        return false;
    }

    const bool found = (spec != Py_None);
    Py_DECREF(spec);
    return found;
}

std::string InstalledPackageChecker::canonicalizePackageName(const std::string& _s)
{
    std::string out;
    out.reserve(_s.size());

    for (char c : _s)
    {
        if (c == '_' || c == '.')
            out.push_back('-');
        else
            out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }

    return out;
}
std::vector<std::string> InstalledPackageChecker::pyListToStringVector(PyObject* _obj)
{
    std::vector<std::string> result;

    if (!_obj || !PyList_Check(_obj))
        return result;

    const Py_ssize_t size = PyList_Size(_obj);
    result.reserve(static_cast<size_t>(size));

    for (Py_ssize_t i = 0; i < size; ++i)
    {
        PyObject* item = PyList_GetItem(_obj, i);
        if (!item)
            continue;

        const char* text = PyUnicode_AsUTF8(item);
        if (text)
            result.emplace_back(text);
    }

    return result;
}

// Helper

std::string InstalledPackageChecker::trim(const std::string& _s)
{
    const auto begin = _s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos)
        return "";

    const auto end = _s.find_last_not_of(" \t\r\n");
    return _s.substr(begin, end - begin + 1);
}

std::string InstalledPackageChecker::normalizeImportName(const std::string& _s)
{
    std::string out;
    out.reserve(_s.size());

    for (char c : _s)
    {
        if (c == '.')
            break;

        if (c == '-')
            out.push_back('_');
        else
            out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }

    return out;
}
