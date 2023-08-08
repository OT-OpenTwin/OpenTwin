#include "SolverElectrostatics.h"

#include <cassert>

void SolverElectrostatics::writeInputFile(std::ofstream& _controlFile)
{
    // Get map of all materials and their corresponding objects
    std::map<std::string, std::list<std::string>> materialsToObjectsMap;
    getMaterialsToObjectsMap(materialsToObjectsMap);

    // Get map of all shapes with potential definition
    std::map<std::string, double> potentialDefinitions;
    getPotentialDefinitions(potentialDefinitions);

    // Now we build (alias) names for each material
    std::map<std::string, std::string> materialNameToAliasMap;
    buildMaterialAliases(materialsToObjectsMap, materialNameToAliasMap);

    // Now we build (alias) names for each potential definition
    std::map<std::string, std::string> potentialNameToAliasMap;
    buildPotentialAliases(potentialDefinitions, potentialNameToAliasMap);

    // Write the groups defining the regions
    writeGroups(_controlFile, materialsToObjectsMap, materialNameToAliasMap, potentialDefinitions, potentialNameToAliasMap);

    // Write the functions defining the materials
    writeFunctions(_controlFile, materialNameToAliasMap);

    // Write the constraints defining the boundary conditions and potentials
    writeConstraints(_controlFile, potentialDefinitions, potentialNameToAliasMap);

    // Write the function space
    writeFunctionSpace(_controlFile);

    // Write the Jacobian
    writeJacobian(_controlFile);

    // Write the integration rules
    writeIntegration(_controlFile);

    // Write the formulation
    writeFormulation(_controlFile);

    // Write the resolution
    writeResolution(_controlFile);

    // Write the post-processing
    writePostProcessing(_controlFile);

    // Write the post-operation
    writePostOperation(_controlFile);
}

void SolverElectrostatics::runSolver(const std::string& tempDirPath)
{
    runSolverExe("model", "EleSta_v", "Map", tempDirPath);
}

void SolverElectrostatics::convertResults(const std::string& tempDirPath)
{
}

void SolverElectrostatics::getMaterialsToObjectsMap(std::map<std::string, std::list<std::string>>& materialsToObjectsMap)
{
    // Here we need to loop through all mesh items and their properties and get their materials
    for (auto item : entityProperties)
    {
        if (meshItemInfo[item.first].getType() == "EntityMeshTetItem")
        {
            EntityPropertiesEntityList* materialProperty = dynamic_cast<EntityPropertiesEntityList*>(item.second.getProperty("Material"));

            if (materialProperty != nullptr)
            {
                std::string materialName = materialProperty->getValueName();
                materialsToObjectsMap[materialName].push_back(meshItemInfo[item.first].getName());
            }
        }
    }
}

void SolverElectrostatics::getPotentialDefinitions(std::map<std::string, double>& potentialDefinitions)
{
    // Here we need to loop through all objects and their properties and check which one has a potential definition
    for (auto item : entityProperties)
    {
        EntityPropertiesDouble *potential = dynamic_cast<EntityPropertiesDouble*>(item.second.getProperty("Electrostatic Potential"));

        if (potential != nullptr)
        {
            potentialDefinitions[meshItemInfo[item.first].getName()] = potential->getValue();
        }
    }
}

void SolverElectrostatics::buildMaterialAliases(std::map<std::string, std::list<std::string>>& materialsToObjectsMap, std::map<std::string, std::string>& materialNameToAliasMap)
{
    // Now we create an alias name "material#n" for each material
    int count = 1;
    for (auto material : materialsToObjectsMap)
    {
        materialNameToAliasMap[material.first] = "material" + std::to_string(count);
        count++;
    }
}

void SolverElectrostatics::buildPotentialAliases(std::map<std::string, double>& potentialDefinitions, std::map<std::string, std::string>& potentialNameToAliasMap)
{
    // Now we create an alias name "potential#n" for each potential definition
    int count = 1;
    for (auto material : potentialDefinitions)
    {
        potentialNameToAliasMap[material.first] = "potential" + std::to_string(count);
        count++;
    }
}

void SolverElectrostatics::writeGroups(std::ofstream& controlFile, 
                                       std::map<std::string, std::list<std::string>>& materialsToObjectsMap, std::map<std::string, std::string>& materialNameToAliasMap,
                                       std::map<std::string, double>& potentialDefinitions, std::map<std::string, std::string>& potentialNameToAliasMap)
{
    controlFile << "Group{\n\n";

    // Write for each material which shapes belong to this material (volume groups)
    std::list<std::string> volumeGroupNames;

    for (auto material : materialsToObjectsMap)
    {
        if (!isPECMaterial(material.first))
        {
            std::string groupName = materialNameToAliasMap[material.first];
            std::list<int> groupItemList = meshVolumeGroupIdList(material.second);
            std::string groupList = getGroupList(groupItemList);

            controlFile << "   " << groupName << " = Region[{" << groupList << "}]; \n";
            volumeGroupNames.push_back(groupName);
        }
    }
    controlFile << "\n";

    // Write the regions for potential definitions (surface groups)
    for (auto potential : potentialDefinitions)
    {
        std::string potentialName = potentialNameToAliasMap[potential.first];
        std::list<int> groupItemList = meshSurfaceGroupIdList(std::list<std::string>{potential.first});
        std::string groupList = getGroupList(groupItemList);

        controlFile << "   " << potentialName << " = Region[{" << groupList << "}]; \n";
    }
    controlFile << "\n";

    // Write the background surface region
    controlFile <<
        "   SurfInf = Region[{" << groupNameToIdMap["#Background"] << "}]; \n\n";

    // Write a list of all non-pec volumes = computation domain
    controlFile << 
        "   Vol_Ele = Region[{" << getGroupList(volumeGroupNames) << "}]; \n";

    // Define some further groups which are later on used for solving
    controlFile <<
        "   Sur_Neu_Ele = Region[{}];\n\n"
        "   Dom_Hgrad_v_Ele = Region[{Vol_Ele, Sur_Neu_Ele}];\n"
        "}\n\n";
}

std::list<int> SolverElectrostatics::meshVolumeGroupIdList(const std::list<std::string>& itemNames)
{
    std::list<int> groupList;

    for (auto item : itemNames)
    {
        // First we strip the name of the mesh
        std::string plainItemName = "*" + item.substr(meshDataName.size() + 1);  // We add *, since this is a volume group

        int groupID = (int) groupNameToIdMap[plainItemName];
        groupList.push_back(groupID);
    }

    return groupList;
}

std::list<int> SolverElectrostatics::meshSurfaceGroupIdList(const std::list<std::string>& itemNames)
{
    std::list<int> groupList;

    for (auto item : itemNames)
    {
        // First we strip the name of the mesh
        std::string plainItemName = "#" + item.substr(meshDataName.size() + 1);  // We add #, since this is a surface group

        int groupID = (int)groupNameToIdMap[plainItemName];
        groupList.push_back(groupID);
    }

    return groupList;
}

std::string SolverElectrostatics::getGroupList(const std::list<int>& list)
{
    std::string groupName;

    for (auto item : list)
    {
        if (!groupName.empty())
        {
            groupName += ",";
        }

        groupName += std::to_string(item);
    }

    return groupName;
}

std::string SolverElectrostatics::getGroupList(const std::list<std::string>& list)
{
    std::string groupName;

    for (auto item : list)
    {
        if (!groupName.empty())
        {
            groupName += ",";
        }

        groupName += item;
    }

    return groupName;
}

void SolverElectrostatics::writeFunctions(std::ofstream& controlFile, std::map<std::string, std::string>& materialNameToAliasMap)
{
    controlFile <<
        "Function {\n"
        "   eps0 = 8.854187818e-12;\n";

    for (auto material : materialNameToAliasMap)
    {
        if (!isPECMaterial(material.first))
        {
            EntityPropertiesDouble* permittivity = dynamic_cast<EntityPropertiesDouble*>(materialProperties[material.first].getProperty("Permittivity (relative)"));

            if (permittivity != nullptr)
            {
                controlFile << "   epsilon[" << material.second << "] = " << permittivity->getValue() << " * eps0;\n";
            }
        }
    }

    controlFile << "}\n\n";
}

void SolverElectrostatics::writeConstraints(std::ofstream& controlFile, std::map<std::string, double>& potentialDefinitions, std::map<std::string, std::string>& potentialNameToAliasMap)
{
    controlFile <<
        "Constraint {\n"
        "  { Name Dirichlet_Ele; Type Assign;\n"
        "    Case {\n";

    for (auto potential : potentialDefinitions)
    {
        std::string potentialName = potentialNameToAliasMap[potential.first];
        std::list<int> groupItemList = meshSurfaceGroupIdList(std::list<std::string>{potential.first});
        std::string groupList = getGroupList(groupItemList);

        controlFile << "      { Region " << potentialName << "; Value " << potential.second << "; }\n";
    }

    controlFile <<
        "      { Region SurfInf; Value 0; }\n"
        "    }\n"
        "  }\n"
        "}\n\n";
}

void SolverElectrostatics::writeFunctionSpace(std::ofstream& controlFile)
{
    controlFile <<
        "FunctionSpace{\n"
        "  { Name Hgrad_v_Ele; Type Form0;\n"
        "    BasisFunction {\n"
        "      { Name sn; NameOfCoef vn; Function BF_Node;\n"
        "        Support Dom_Hgrad_v_Ele; Entity NodesOf[All]; }\n"
        "    }\n"
        "    Constraint {\n"
        "      { NameOfCoef vn; EntityType NodesOf; NameOfConstraint Dirichlet_Ele; }\n"
        "    }\n"
        "  }\n"
        "}\n\n";
}

void SolverElectrostatics::writeJacobian(std::ofstream& controlFile)
{
    controlFile <<
        "Jacobian {\n"
        "  { Name Vol;\n"
        "    Case {\n"
        "      { Region All; Jacobian Vol; }\n"
        "    }\n"
        "  }\n"
        "  { Name Sur;\n"
        "    Case {\n"
        "      { Region All; Jacobian Sur; }\n"
        "    }\n"
        "  }\n"
        "}\n\n";
}

void SolverElectrostatics::writeIntegration(std::ofstream& controlFile)
{
    controlFile <<
        "Integration {\n"
        "  { Name Int;\n"
        "    Case{ { Type Gauss;\n"
        "            Case { { GeoElement Line; NumberOfPoints  4; }\n"
        "                   { GeoElement Triangle; NumberOfPoints  4; }\n"
        "                   { GeoElement Quadrangle; NumberOfPoints  4; }\n"
        "                   { GeoElement Tetrahedron; NumberOfPoints  15; }\n"
        "                   { GeoElement Hexahedron; NumberOfPoints  34; }\n"
        "                   { GeoElement Prism; NumberOfPoints  21; } }\n"
        "          }\n"
        "    }\n"
        "  }\n"
        "}\n\n";
}

void SolverElectrostatics::writeFormulation(std::ofstream& controlFile)
{
    controlFile <<
        "Formulation {\n"
        "  { Name Electrostatics_v; Type FemEquation; \n"
        "    Quantity {\n"
        "      { Name v; Type Local; NameOfSpace Hgrad_v_Ele; }\n"
        "    }\n"
        "    Equation {\n"
        "      Integral { [epsilon[] * Dof{d v} , {d v}] ;\n"
        "        In Vol_Ele; Jacobian Vol; Integration Int; }\n"
        "    }\n"
        "  }\n"
        "}\n\n";
}

void SolverElectrostatics::writeResolution(std::ofstream& controlFile)
{
    controlFile <<
        "Resolution { \n"
        "  { Name EleSta_v;\n"
        "    System {\n"
        "      { Name Sys_Ele; NameOfFormulation Electrostatics_v; }\n"
        "    }\n"
        "    Operation {\n"
        "      Generate[Sys_Ele]; Solve[Sys_Ele]; SaveSolution[Sys_Ele];\n"
        "    }\n"
        "  }\n"
        "}\n\n";
}

void SolverElectrostatics::writePostProcessing(std::ofstream& controlFile)
{
    controlFile <<
        "PostProcessing { \n"
        "  { Name EleSta_v; NameOfFormulation Electrostatics_v; \n"
        "    Quantity { \n"
        "      { Name v; Value { \n"
        "          Term { [{v}] ; In Dom_Hgrad_v_Ele; Jacobian Vol; } \n"
        "        }\n"
        "      }\n"
        "      { Name e; Value { \n"
        "          Term { [-{d v}] ; In Dom_Hgrad_v_Ele; Jacobian Vol; } \n"
        "        }\n"
        "      }\n"
        "      { Name d; Value { \n"
        "          Term { [-epsilon[] * {d v}] ; In Dom_Hgrad_v_Ele; Jacobian Vol; } \n"
        "        }\n"
        "      }\n\n"
        "      { Name W; Value { Local { [{v}] ; In Dom_Hgrad_v_Ele; } } }\n"
        "    }\n"
        "  }\n"
        "}\n\n";
}

void SolverElectrostatics::writePostOperation(std::ofstream& controlFile)
{
    controlFile <<
        "PostOperation { \n"
        "  { Name Map; NameOfPostProcessing EleSta_v; \n"
        "     Operation { \n"
        "       Print[v, OnElementsOf Vol_Ele, Format Table, File \"potential.pos\"]; \n"
        "       Print[e, OnElementsOf Vol_Ele, Format Table, File \"efield.pos\"]; \n"
        "     } \n"
        "  } \n"
        "} \n\n";
}
