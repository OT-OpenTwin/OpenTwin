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

    // Write the Jacobian
    writeJacobian(_controlFile);

    // Write the integration rules
    writeIntegration(_controlFile);

    // Write the function space
    writeFunctionSpace(_controlFile);

    // Write the formulation
    writeFormulation(_controlFile);

    // Write the resolution
    writeResolution(_controlFile);

    // Write the post-processing
    writePostProcessing(_controlFile);

    // Write the post-operation
    writePostOperation(_controlFile);
}

std::string SolverElectrostatics::runSolver(const std::string& tempDirPath, ot::components::UiComponent* uiComponent)
{
    runSolverExe("model", "EleSta_v", "Map", tempDirPath, uiComponent);

    return solverOutput.str();
}

void SolverElectrostatics::convertResults(const std::string& tempDirPath)
{
    // We first need to convert the potential result file to a vtk file
    convertPotential(tempDirPath);

    // Now we convert the electric field results to a vtk file
    convertEfield(tempDirPath);
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
        "   SurfInf = Region[{" << groupNameToIdMap["$BOUNDARY"] << "}]; \n\n";

    // Write a list of all non-pec volumes = computation domain
    controlFile <<
        "   DomainCC_Ele = Region[{" << getGroupList(volumeGroupNames) << "}]; \n"
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
    controlFile << "Function {\n";

    for (auto material : materialNameToAliasMap)
    {
        if (!isPECMaterial(material.first))
        {
            EntityPropertiesDouble* permittivity = dynamic_cast<EntityPropertiesDouble*>(materialProperties[material.first].getProperty("Permittivity (relative)"));

            if (permittivity != nullptr)
            {
                controlFile << "   epsr[" << material.second << "] = " << permittivity->getValue() << "; \n";
            }
        }
    }

    controlFile << "}\n\n";
}

void SolverElectrostatics::writeConstraints(std::ofstream& controlFile, std::map<std::string, double>& potentialDefinitions, std::map<std::string, std::string>& potentialNameToAliasMap)
{
    controlFile <<
        "Constraint {\n"
        "  { Name ElectricScalarPotential; Type Assign;\n"
        "    Case {\n";

    for (auto potential : potentialDefinitions)
    {
        std::string potentialName = potentialNameToAliasMap[potential.first];
        std::list<int> groupItemList = meshSurfaceGroupIdList(std::list<std::string>{potential.first});
        std::string groupList = getGroupList(groupItemList);

        controlFile << "      { Region " << potentialName << "; Value " << potential.second << "; }\n";
    }

    controlFile <<
        "      { Region SurfInf; Value 0.; }\n"
        "    }\n"
        "  }\n"
        "}\n\n";
}

void SolverElectrostatics::writeFunctionSpace(std::ofstream& controlFile)
{
    controlFile <<
        "eps0 = 8.854187818e-12;\n\n"
        "Group{\n"
        "  DefineGroup[Domain_Ele, DomainCC_Ele, DomainC_Ele];\n"
        "}\n\n"
        "Function{\n"
        "  DefineFunction[epsr];\n"
        "}\n\n"
        "FunctionSpace{\n"
        "  { Name Hgrad_v_Ele; Type Form0;\n"
        "    BasisFunction {\n"
        "      { Name sn; NameOfCoef vn; Function BF_Node;\n"
        "        Support DomainCC_Ele; Entity NodesOf[All]; }\n"
        "    }\n"
        "    Constraint {\n"
        "      { NameOfCoef vn; EntityType NodesOf;\n"
        "        NameOfConstraint ElectricScalarPotential; }\n"
        "    }\n"
        "  }\n"
        "}\n\n";
}

void SolverElectrostatics::writeJacobian(std::ofstream& controlFile)
{
    controlFile <<
        "Group{ \n"
        "  DefineGroup[DomainInf]; \n"
        "  DefineVariable[Val_Rint, Val_Rext]; \n"
        "}\n\n"
        "Jacobian{ \n"
        "  { Name Vol; \n"
        "    Case { { Region DomainInf; \n"
        "             Jacobian VolSphShell {Val_Rint, Val_Rext}; }\n"
        "           { Region All; Jacobian Vol; }\n"
        "    }\n"
        "  }\n"
        "}\n\n";
}

void SolverElectrostatics::writeIntegration(std::ofstream& controlFile)
{
    controlFile <<
        "Integration {\n"
        "   { Name GradGrad; \n"
        "     Case{ {Type Gauss; \n"
        "            Case { { GeoElement Triangle;    NumberOfPoints  4; }\n"
        "                   { GeoElement Quadrangle;  NumberOfPoints  4; }\n"
        "                   { GeoElement Tetrahedron; NumberOfPoints  4; }\n"
        "                   { GeoElement Hexahedron;  NumberOfPoints  6; }\n"
        "                   { GeoElement Prism;       NumberOfPoints  9; } }\n"
        "           }\n"
        "         }\n"
        "   }\n"
        "   { Name CurlCurl; \n"
        "     Case{ {Type Gauss; \n"
        "            Case { { GeoElement Triangle;    NumberOfPoints  4; }\n"
        "                   { GeoElement Quadrangle;  NumberOfPoints  4; }\n"
        "                   { GeoElement Tetrahedron; NumberOfPoints  4; }\n"
        "                   { GeoElement Hexahedron;  NumberOfPoints  6; }\n"
        "                   { GeoElement Prism;       NumberOfPoints  9; } }\n"
        "            }\n"
        "         }\n"
        "   }\n"
        "}\n\n";
}

void SolverElectrostatics::writeFormulation(std::ofstream& controlFile)
{
    controlFile <<
        "Formulation{\n"
        "  { Name Electrostatics_v; Type FemEquation;\n"
        "    Quantity {\n"
        "      { Name v; Type Local; NameOfSpace Hgrad_v_Ele; }\n"
        "    }\n"
        "    Equation {\n"
        "      Galerkin { [epsr[] * Dof{d v} , {d v}] ; In DomainCC_Ele;\n"
        "                 Jacobian Vol; Integration GradGrad; }\n"
        "    }\n"
        "  }\n"
        "}\n\n";
}

void SolverElectrostatics::writeResolution(std::ofstream& controlFile)
{
    controlFile <<
        "Resolution{\n"
        "  { Name EleSta_v;\n"
        "    System {\n"
        "      { Name Sys_Ele; NameOfFormulation Electrostatics_v; }\n"
        "    }\n"
        "    Operation {\n"
        "      Generate[Sys_Ele]; Solve[Sys_Ele]; SaveSolution[Sys_Ele];\n"
        "    }\n"
        " }\n"
        "}\n\n";
}

void SolverElectrostatics::writePostProcessing(std::ofstream& controlFile)
{
    controlFile <<
        "PostProcessing{\n"
        "  { Name EleSta_v; NameOfFormulation Electrostatics_v;\n"
        "    Quantity {\n"
        "      { Name v;\n"
        "        Value {\n"
        "          Local { [{v}] ; In DomainCC_Ele; Jacobian Vol; }\n"
        "        }\n"
        "      }\n"
        "      { Name e;\n"
        "        Value {\n"
        "          Local { [-{d v}] ; In DomainCC_Ele; Jacobian Vol; }\n"
        "        }\n"
        "      }\n"
        "      { Name d;\n"
        "        Value {\n"
        "          Local { [-eps0 * epsr[] * {d v}] ; In DomainCC_Ele;\n"
        "                                             Jacobian Vol; }\n"
        "        }\n"
        "      }\n"
        "    }\n"
        "  }\n"
        " }\n\n";
}

void SolverElectrostatics::writePostOperation(std::ofstream& controlFile)
{
    controlFile <<
        "PostOperation { \n"
        "  { Name Map; NameOfPostProcessing EleSta_v; \n"
        "     Operation { \n"
        "       Print[v, OnElementsOf DomainCC_Ele, Format Table, File \"potential.pos\"]; \n"
        "       Print[e, OnElementsOf DomainCC_Ele, Format Table, File \"efield.pos\"]; \n"
        "     } \n"
        "  } \n"
        "} \n\n";
}

void SolverElectrostatics::convertPotential(const std::string& tempDirPath)
{
    // Open the potential file and read nodes (with potentials) and cells into intermediate data structures
    std::string potentialFileName = tempDirPath + "\\potential.pos";
    std::ifstream potentialFile(potentialFileName);

    std::map<std::string, size_t> nodeToIndexMap;
    std::list<std::string> nodeList;
    std::list<std::string> potentialList;
    std::list<std::vector<size_t>> cellList;

    size_t nodeIndex = 0;

    // Now read the file line by line
    const int elementsPerRow = 21;

    std::vector<std::string> elements;
    elements.resize(elementsPerRow);

    bool fileEndReached = false;
    while (!fileEndReached)
    {
        for (int index = 0; index < elementsPerRow; index++)
        {
            if (!(potentialFile >> elements[index]))
            {
                // We have reached the end of the file
                fileEndReached = true;
                break;
            }
        }

        if (!fileEndReached)
        {
            // Now we process the line
            std::string n1 = elements[ 2] + " " + elements[ 3] + " " + elements[ 4];
            std::string n2 = elements[ 5] + " " + elements[ 6] + " " + elements[ 7];
            std::string n3 = elements[ 8] + " " + elements[ 9] + " " + elements[10];
            std::string n4 = elements[11] + " " + elements[12] + " " + elements[13];

            size_t indexN1 = getOrAddNode(n1, elements[17], nodeToIndexMap, nodeList, potentialList, nodeIndex);
            size_t indexN2 = getOrAddNode(n2, elements[18], nodeToIndexMap, nodeList, potentialList, nodeIndex);
            size_t indexN3 = getOrAddNode(n3, elements[19], nodeToIndexMap, nodeList, potentialList, nodeIndex);
            size_t indexN4 = getOrAddNode(n4, elements[20], nodeToIndexMap, nodeList, potentialList, nodeIndex);

            std::vector<size_t> cell{ indexN1, indexN2, indexN3, indexN4 };
            cellList.push_back(cell);
        }
    }

    potentialFile.close();

    // Now write the vtk file information based on the intermediate data structures
    std::string vtkFileName = tempDirPath + "\\potential.vtu";
    std::ofstream vtkFile(vtkFileName);

    vtkFile << "# vtk DataFile Version 2.0" << std::endl;
    vtkFile << "Electrostatic potential" << std::endl;
    vtkFile << "ASCII" << std::endl;
    vtkFile << "DATASET UNSTRUCTURED_GRID" << std::endl << std::endl;

    vtkFile << "POINTS " << nodeList.size() << " float" << std::endl;
    for (auto node : nodeList)
    {
        vtkFile << node << std::endl;
    }

    vtkFile << std::endl << "CELLS " << cellList.size() << " " << 5 * cellList.size() << std::endl;
    for (auto cell : cellList)
    {
        vtkFile << "4 " << cell[0] << " " << cell[1] << " " << cell[2] << " " << cell[3] << std::endl;
    }

    vtkFile << std::endl << "CELL_TYPES " << cellList.size() << std::endl;
    for (auto cell : cellList)
    {
        vtkFile << "10" << std::endl;
    }

    vtkFile << std::endl << "POINT_DATA " << nodeList.size() << std::endl;
    vtkFile << "SCALARS scalars float 1" << std::endl;
    vtkFile << "LOOKUP_TABLE default" << std::endl;

    for (auto potential : potentialList)
    {
        vtkFile << potential << std::endl;
    }

    vtkFile.close();
}

size_t SolverElectrostatics::getOrAddNode(const std::string& node, const std::string& potential, 
                                          std::map<std::string, size_t> &nodeToIndexMap, std::list<std::string> &nodeList, std::list<std::string> &potentialList,
                                          size_t &nodeIndex)
{
    size_t index = 0;

    if (nodeToIndexMap.count(node) == 0)
    {
        index = nodeIndex;
        nodeIndex++;

        nodeToIndexMap[node] = index;

        nodeList.push_back(node);
        potentialList.push_back(potential);
    }
    else
    {
        index = nodeToIndexMap[node];
    }

    return index;
}

void SolverElectrostatics::convertEfield(const std::string& tempDirPath)
{

}
