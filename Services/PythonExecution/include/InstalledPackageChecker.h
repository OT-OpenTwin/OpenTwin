#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <Python.h>

class InstalledPackageChecker
{
public:
    bool refreshPackageCaches();

    bool isRequirementSatisfied(const std::string& _requirementLine) const;

    std::vector<std::string> findMissingDistributionsForScriptImports(const std::string& _scriptText) const;
    std::string buildRequirementsText() const;
    uint64_t getNumberOfInstalledPackages();
private:
    std::unordered_map<std::string, std::string> m_installedPackageVersionsByCanonicalName;
    //import names and distribution names are not guaranteed to be the same.
    std::unordered_map<std::string, std::vector<std::string>> m_importToDistributionsByCanonicalImport;

    static std::string trim(const std::string& _s);
    static std::string normalizeImportName(const std::string& _s);
    static std::vector<std::string> pyListToStringVector(PyObject* _obj);

    bool isInstalledDistributionCanonical(const std::string& _canonicalName) const;
    bool isImportableModule(const std::string& _moduleName) const;

    std::string canonicalizePackageName(const std::string& _s);
};

