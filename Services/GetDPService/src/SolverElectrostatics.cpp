#include "SolverElectrostatics.h"
#include "Application.h"
#include "EntityVis2D3D.h"
#include "EntityVisUnstructuredScalarSurface.h"
#include "EntityVisUnstructuredScalarVolume.h"
#include "EntityVisUnstructuredVectorVolume.h"
#include "EntityBinaryData.h"
#include "EntityResultUnstructuredMesh.h"
#include "EntityResultUnstructuredMeshData.h"

#include "OTServiceFoundation/ModelComponent.h"

#include <cassert>

void SolverElectrostatics::writeInputFile(std::ofstream& _controlFile, Application *app)
{
    // Get map of all materials and their corresponding objects
    std::map<std::string, std::list<std::string>> materialsToObjectsMap;
    getMaterialsToObjectsMap(materialsToObjectsMap, app);

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

void SolverElectrostatics::convertResults(const std::string& tempDirPath, Application* app, EntityBase* solverEntity)
{
    long long globalVisualizationMeshID = -1;
    long long globalVisualizationMeshVersion = -1;

    // We first need to convert the potential result file to a vtk file
    convertPotential(tempDirPath, app, solverEntity, globalVisualizationMeshID, globalVisualizationMeshVersion);

    // Now we convert the electric field results to a vtk file
    convertEfield(tempDirPath, app, solverEntity, globalVisualizationMeshID, globalVisualizationMeshVersion);
}

void SolverElectrostatics::getMaterialsToObjectsMap(std::map<std::string, std::list<std::string>>& materialsToObjectsMap, Application* app)
{
    // Here we need to loop through all mesh items and their properties and get their materials
    for (auto item : entityProperties)
    {
        if (meshItemInfo[item.first].getType() == "EntityMeshTetItem")
        {
            EntityPropertiesEntityList* materialProperty = dynamic_cast<EntityPropertiesEntityList*>(item.second.getProperty("Material"));

            if (materialProperty != nullptr)
            {
                std::string materialName = app->modelComponent()->getCurrentMaterialName(materialProperty);
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
        "      { Region SurfInf; Value 0.0; }\n"
        "    }\n"
        "  }\n";

    controlFile <<
        "  { Name ElectricScalarPotential_d2; Type Assign;\n"
        "    Case {\n";

    for (auto potential : potentialDefinitions)
    {
        std::string potentialName = potentialNameToAliasMap[potential.first];
        std::list<int> groupItemList = meshSurfaceGroupIdList(std::list<std::string>{potential.first});
        std::string groupList = getGroupList(groupItemList);

        controlFile << "      { Region " << potentialName << "; Value 0.0; }\n";
    }

    controlFile <<
        "      { Region SurfInf; Value 0.0; }\n"
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
        "      { Name s2; NameOfCoef v2; Function BF_Node_2E;\n"
        "        Support DomainCC_Ele; Entity EdgesOf[All]; }\n"
        "    }\n"
        "    Constraint {\n"
        "      { NameOfCoef vn; EntityType NodesOf;\n"
        "        NameOfConstraint ElectricScalarPotential; }\n"
        "      { NameOfCoef v2; EntityType NodesOf;\n"
        "        NameOfConstraint ElectricScalarPotential_d2; }\n"
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
        "  { Name Sur; \n"
        "    Case{ { Region All; Jacobian Sur; }\n"
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
        "            Case { { GeoElement Line;          NumberOfPoints   2; }\n"
        "                   { GeoElement Triangle;      NumberOfPoints   4; }\n"
        "                   { GeoElement Quadrangle;    NumberOfPoints   4; }\n"
        "                   { GeoElement Tetrahedron;   NumberOfPoints   4; }\n"
        "                   { GeoElement Hexahedron;    NumberOfPoints   6; }\n"
        "                   { GeoElement Prism;         NumberOfPoints   9; }\n"
        "                   { GeoElement Line2;         NumberOfPoints   3; }\n"
        "                   { GeoElement Triangle2;     NumberOfPoints   6; }\n"
        "                   { GeoElement Quadrangle2;   NumberOfPoints   7; }\n"
        "                   { GeoElement Tetrahedron2;  NumberOfPoints  15; }\n"
        "                   { GeoElement Hexahedron2;   NumberOfPoints  34; }\n"
        "                   { GeoElement Prism2;        NumberOfPoints  21; } }\n"
        "           }\n"
        "         }\n"
        "   }\n"
        "   { Name CurlCurl; \n"
        "     Case{ {Type Gauss; \n"
        "            Case { { GeoElement Triangle;     NumberOfPoints  3; }\n"
        "                   { GeoElement Quadrangle;   NumberOfPoints  4; }\n"
        "                   { GeoElement Tetrahedron;  NumberOfPoints  4; }\n"
        "                   { GeoElement Hexahedron;   NumberOfPoints  6; }\n"
        "                   { GeoElement Prism;        NumberOfPoints  9; }\n"
        "                   { GeoElement Line2;        NumberOfPoints  3; }\n"
        "                   { GeoElement Triangle2;    NumberOfPoints  6; }\n"
        "                   { GeoElement Quadrangle2;  NumberOfPoints  7; }\n"
        "                   { GeoElement Tetrahedron2; NumberOfPoints 15; }\n"
        "                   { GeoElement Hexahedron2;  NumberOfPoints 34; }\n"
        "                   { GeoElement Prism2;       NumberOfPoints 21; } }\n"
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
        "      Integral { [epsr[] * Dof{Grad v} , {Grad v}] ; In DomainCC_Ele;\n"
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
        "       Print[e, OnElementsOf DomainCC_Ele, Format Table, File \"efield.pos\"]; \n";

    for (auto item : groupNameToIdMap)
    {
        // Now we create a file with the potential on the surface of this object
        if (item.first[0] == '#')
        {
            controlFile <<
                "       Print[v, OnElementsOf Region[{" << item.second << "}], Format Table, File \"potential#" << item.second << ".pos\"]; \n";
        }
    }

    controlFile <<
        "     } \n"
        "  } \n"
        "} \n\n";
}

void SolverElectrostatics::convertPotential(const std::string& tempDirPath, Application* app, EntityBase* solverEntity, long long& globalVisualizationMeshID, long long& globalVisualizationMeshVersion)
{
    std::map<std::string, std::string> nodeToPotentialMap;

    convertGlobalPotential(tempDirPath, nodeToPotentialMap, app, solverEntity, globalVisualizationMeshID, globalVisualizationMeshVersion);
    convertSurfacePotentials(tempDirPath, nodeToPotentialMap, app, solverEntity);
}

void SolverElectrostatics::convertGlobalPotential(const std::string& tempDirPath, std::map<std::string, std::string>& nodeToPotentialMap, Application* app, EntityBase*solverEntity,
                                                  long long &globalVisualizationMeshID, long long &globalVisualizationMeshVersion)
{
    // Open the potential file and read nodes (with potentials) and cells into intermediate data structures
    std::string potentialFileName = tempDirPath + "\\potential.pos";
    std::ifstream potentialFile(potentialFileName);

    std::map<std::string, size_t> nodeToIndexMap;
    std::list<std::string> nodeList;
    std::list<std::string> potentialList;
    std::list<std::vector<size_t>> cellList;
    size_t cellListSize = 0;

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
            std::string n1 = elements[2] + " " + elements[3] + " " + elements[4];
            std::string n2 = elements[5] + " " + elements[6] + " " + elements[7];
            std::string n3 = elements[8] + " " + elements[9] + " " + elements[10];
            std::string n4 = elements[11] + " " + elements[12] + " " + elements[13];

            size_t indexN1 = getOrAddNode(n1, elements[17], nodeToIndexMap, nodeList, potentialList, nodeIndex, nodeToPotentialMap);
            size_t indexN2 = getOrAddNode(n2, elements[18], nodeToIndexMap, nodeList, potentialList, nodeIndex, nodeToPotentialMap);
            size_t indexN3 = getOrAddNode(n3, elements[19], nodeToIndexMap, nodeList, potentialList, nodeIndex, nodeToPotentialMap);
            size_t indexN4 = getOrAddNode(n4, elements[20], nodeToIndexMap, nodeList, potentialList, nodeIndex, nodeToPotentialMap);

            std::vector<size_t> cell{ indexN1, indexN2, indexN3, indexN4 };
            cellList.push_back(cell);

            cellListSize += 6;
        }
    }

    potentialFile.close();

    // Create the global mesh item if needed
    if (globalVisualizationMeshID == -1 || globalVisualizationMeshVersion == -1)
    {
        storeMesh(4, 10, nodeList, cellList, cellListSize, app, solverEntity, globalVisualizationMeshID, globalVisualizationMeshVersion);
    }

    // Create the potential data item 
    long long scalarDataID = -1;
    long long scalarDataVersion = -1;
    storeMeshScalarData(nodeList.size(), cellList.size(), potentialList, app, solverEntity, scalarDataID, scalarDataVersion);

    // Create the potential volume item
    EntityVis2D3D* visualizationEntity = new EntityVisUnstructuredScalarVolume(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);;

    visualizationEntity->setResultType(EntityResultBase::UNSTRUCTURED_SCALAR);
    visualizationEntity->setName(solverEntity->getName() + "/Results/Potential/Volume");
    visualizationEntity->setEditable(true);
    visualizationEntity->setInitiallyHidden(true);

    visualizationEntity->createProperties();

    visualizationEntity->setSource(scalarDataID, scalarDataVersion);
    visualizationEntity->setMesh(globalVisualizationMeshID, globalVisualizationMeshVersion);

    visualizationEntity->StoreToDataBase();

    app->modelComponent()->addNewTopologyEntity(visualizationEntity->getEntityID(), visualizationEntity->getEntityStorageVersion(), false);

    delete visualizationEntity;
    visualizationEntity = nullptr;

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

void SolverElectrostatics::convertSurfacePotentials(const std::string& tempDirPath, std::map<std::string, std::string> &nodeToPotentialMap, Application* app, EntityBase* solverEntity)
{
    for (auto item : groupNameToIdMap)
    {
        // Now we create a file with the potential on the surface of this object
        if (item.first[0] == '#')
        {
            std::string potentialFileName = tempDirPath + "\\potential#" + std::to_string(item.second) + ".pos";
            std::ifstream potentialFile(potentialFileName);

            std::map<std::string, size_t> nodeToIndexMap;
            std::list<std::string> nodeList;
            std::list<std::string> potentialList;
            std::list<std::vector<size_t>> cellList;
            size_t cellListSize = 0;

            const int elementsPerRow = 17;

            std::vector<std::string> elements;
            elements.resize(elementsPerRow);

            size_t nodeIndex = 0;

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
                    std::string n1 = elements[2] + " " + elements[3] + " " + elements[4];
                    std::string n2 = elements[5] + " " + elements[6] + " " + elements[7];
                    std::string n3 = elements[8] + " " + elements[9] + " " + elements[10];

                    size_t indexN1 = getOrAddNode(n1, "", nodeToIndexMap, nodeList, potentialList, nodeIndex, nodeToPotentialMap);
                    size_t indexN2 = getOrAddNode(n2, "", nodeToIndexMap, nodeList, potentialList, nodeIndex, nodeToPotentialMap);
                    size_t indexN3 = getOrAddNode(n3, "", nodeToIndexMap, nodeList, potentialList, nodeIndex, nodeToPotentialMap);

                    std::vector<size_t> cell{ indexN1, indexN2, indexN3 };
                    cellList.push_back(cell);

                    cellListSize += 5;
                }
            }

            potentialFile.close();

            // Now we extract the potential data for the surface
            potentialList.clear();

            for (auto node : nodeList)
            {
                potentialList.push_back(nodeToPotentialMap[node]);
            }

            // Create the surface mesh item if needed
            long long visualizationMeshID = -1;
            long long visualizationMeshVersion = -1;
            storeMesh(3, 5, nodeList, cellList, cellListSize, app, solverEntity, visualizationMeshID, visualizationMeshVersion);

            // Create the surface potential data item 
            long long scalarDataID = -1;
            long long scalarDataVersion = -1;
            storeMeshScalarData(nodeList.size(), cellList.size(), potentialList, app, solverEntity, scalarDataID, scalarDataVersion);

            // Create the potential volume item
            EntityVis2D3D* visualizationEntity = new EntityVisUnstructuredScalarSurface(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);;

            visualizationEntity->setResultType(EntityResultBase::UNSTRUCTURED_SCALAR);
            visualizationEntity->setName(solverEntity->getName() + "/Results/Potential/Surface/" + item.first.substr(1));
            visualizationEntity->setEditable(true);
            visualizationEntity->setInitiallyHidden(true);

            visualizationEntity->createProperties();

            visualizationEntity->setSource(scalarDataID, scalarDataVersion);
            visualizationEntity->setMesh(visualizationMeshID, visualizationMeshVersion);

            visualizationEntity->StoreToDataBase();

            app->modelComponent()->addNewTopologyEntity(visualizationEntity->getEntityID(), visualizationEntity->getEntityStorageVersion(), false);

            delete visualizationEntity;
            visualizationEntity = nullptr;

            // Write the vtk file
            std::string vtkFileName = tempDirPath + "\\potential#" + std::to_string(item.second) + ".vtu";
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

            vtkFile << std::endl << "CELLS " << cellList.size() << " " << 4 * cellList.size() << std::endl;
            for (auto cell : cellList)
            {
                vtkFile << "3 " << cell[0] << " " << cell[1] << " " << cell[2] << std::endl;
            }

            vtkFile << std::endl << "CELL_TYPES " << cellList.size() << std::endl;
            for (auto cell : cellList)
            {
                vtkFile << "5" << std::endl;
            }

            vtkFile << std::endl << "POINT_DATA " << nodeList.size() << std::endl;
            vtkFile << "SCALARS scalars float 1" << std::endl;
            vtkFile << "LOOKUP_TABLE default" << std::endl;

            for (auto node : nodeList)
            {
                vtkFile << nodeToPotentialMap[node] << std::endl;
            }
        }
    }
}

size_t SolverElectrostatics::getOrAddNode(const std::string& node, const std::string& potential, 
                                          std::map<std::string, size_t> &nodeToIndexMap, std::list<std::string> &nodeList, std::list<std::string> &potentialList,
                                          size_t &nodeIndex, std::map<std::string, std::string>& nodeToPotentialMap)
{
    size_t index = 0;

    if (nodeToIndexMap.count(node) == 0)
    {
        index = nodeIndex;
        nodeIndex++;

        nodeToIndexMap[node] = index;

        nodeList.push_back(node);
        potentialList.push_back(potential);

        if (!potential.empty())
        {
            nodeToPotentialMap[node] = potential;
        }
    }
    else
    {
        index = nodeToIndexMap[node];
    }

    return index;
}

void SolverElectrostatics::convertEfield(const std::string& tempDirPath, Application* app, EntityBase* solverEntity, long long& globalVisualizationMeshID, long long& globalVisualizationMeshVersion)
{
    // Open the potential file and read nodes (with potentials) and cells into intermediate data structures
    std::string potentialFileName = tempDirPath + "\\efield.pos";
    std::ifstream potentialFile(potentialFileName);

    std::map<std::string, size_t> nodeToIndexMap;
    std::list<std::string> nodeList;
    std::list<std::vector<size_t>> cellList;
    std::list<std::string> vectorList;
    std::list<double> magnitudeList;
    size_t cellListSize = 0;

    size_t nodeIndex = 0;

    // Now read the file line by line
    const int elementsPerRow = 29;

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
            std::string n1 = elements[2] + " " + elements[3] + " " + elements[4];
            std::string n2 = elements[5] + " " + elements[6] + " " + elements[7];
            std::string n3 = elements[8] + " " + elements[9] + " " + elements[10];
            std::string n4 = elements[11] + " " + elements[12] + " " + elements[13];

            size_t indexN1 = getOrAddCellNode(n1, nodeToIndexMap, nodeList, nodeIndex);
            size_t indexN2 = getOrAddCellNode(n2, nodeToIndexMap, nodeList, nodeIndex);
            size_t indexN3 = getOrAddCellNode(n3, nodeToIndexMap, nodeList, nodeIndex);
            size_t indexN4 = getOrAddCellNode(n4, nodeToIndexMap, nodeList, nodeIndex);

            double x1 = atof(elements[17].c_str());
            double y1 = atof(elements[18].c_str());
            double z1 = atof(elements[19].c_str());

            double x2 = atof(elements[20].c_str());
            double y2 = atof(elements[21].c_str());
            double z2 = atof(elements[22].c_str());

            double x3 = atof(elements[23].c_str());
            double y3 = atof(elements[24].c_str());
            double z3 = atof(elements[25].c_str());

            double x4 = atof(elements[26].c_str());
            double y4 = atof(elements[27].c_str());
            double z4 = atof(elements[28].c_str());

            double xAverage = 0.25 * (x1 + x2 + x3 + x4);
            double yAverage = 0.25 * (y1 + y2 + y3 + y4);
            double zAverage = 0.25 * (z1 + z2 + z3 + z4);

            double magnitude = sqrt(xAverage * xAverage + yAverage * yAverage + zAverage * zAverage);

            std::string averageVector = std::to_string(xAverage) + " " + std::to_string(yAverage) + " " + std::to_string(zAverage);

            std::vector<size_t> cell{ indexN1, indexN2, indexN3, indexN4 };
            cellList.push_back(cell);
            vectorList.push_back(averageVector);
            magnitudeList.push_back(magnitude);

            cellListSize += 6;
        }
    }

    potentialFile.close();

    // Create the global mesh item if needed
    if (globalVisualizationMeshID == -1 || globalVisualizationMeshVersion == -1)
    {
        storeMesh(4, 10, nodeList, cellList, cellListSize, app, solverEntity, globalVisualizationMeshID, globalVisualizationMeshVersion);
    }

    // Create the efield data item 
    long long vectorDataID = -1;
    long long vectorDataVersion = -1;
    storeMeshVectorData(nodeList.size(), cellList.size(), magnitudeList, vectorList, app, solverEntity, vectorDataID, vectorDataVersion);

    // Create the efield item
    EntityVis2D3D* visualizationEntity = new EntityVisUnstructuredVectorVolume(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);;

    visualizationEntity->setResultType(EntityResultBase::UNSTRUCTURED_VECTOR);
    visualizationEntity->setName(solverEntity->getName() + "/Results/E-Field");
    visualizationEntity->setEditable(true);
    visualizationEntity->setInitiallyHidden(true);

    visualizationEntity->createProperties();

    visualizationEntity->setSource(vectorDataID, vectorDataVersion);
    visualizationEntity->setMesh(globalVisualizationMeshID, globalVisualizationMeshVersion);

    visualizationEntity->StoreToDataBase();

    app->modelComponent()->addNewTopologyEntity(visualizationEntity->getEntityID(), visualizationEntity->getEntityStorageVersion(), false);

    delete visualizationEntity;
    visualizationEntity = nullptr;

    // Now write the vtk file information based on the intermediate data structures
    std::string vtkFileName = tempDirPath + "\\efield.vtu";
    std::ofstream vtkFile(vtkFileName);

    vtkFile << "# vtk DataFile Version 2.0" << std::endl;
    vtkFile << "Electrostatic field strength" << std::endl;
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

    vtkFile << std::endl << "CELL_DATA " << vectorList.size() << std::endl;
    vtkFile << "SCALARS scalars float 1" << std::endl;
    vtkFile << "LOOKUP_TABLE default" << std::endl;

    for (auto magnitude : magnitudeList)
    {
        vtkFile << magnitude << std::endl;
    }

    vtkFile << "VECTORS vectors float" << std::endl;

    for (auto vector : vectorList)
    {
        vtkFile << vector << std::endl;
    }

    vtkFile.close();
}

size_t SolverElectrostatics::getOrAddCellNode(const std::string& node, std::map<std::string, size_t>& nodeToIndexMap, std::list<std::string>& nodeList, size_t& nodeIndex)
{
    size_t index = 0;

    if (nodeToIndexMap.count(node) == 0)
    {
        index = nodeIndex;
        nodeIndex++;

        nodeToIndexMap[node] = index;

        nodeList.push_back(node);
    }
    else
    {
        index = nodeToIndexMap[node];
    }

    return index;
}

void SolverElectrostatics::storeMesh(int numberNodes, int cellType, std::list<std::string>& nodeList, std::list<std::vector<size_t>>& cellList, size_t& cellListSize, Application* app, EntityBase* solverEntity, long long& visualizationMeshID, long long& visualizationMeshVersion)
{
    // Create the global mesh item if needed
    EntityBinaryData* xcoord = new EntityBinaryData(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
    EntityBinaryData* ycoord = new EntityBinaryData(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
    EntityBinaryData* zcoord = new EntityBinaryData(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
    EntityBinaryData* cells = new EntityBinaryData(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);

    double* xc = new double[nodeList.size()];
    double* yc = new double[nodeList.size()];
    double* zc = new double[nodeList.size()];

    size_t index = 0;
    for (auto node : nodeList)
    {
        std::stringstream ss(node);

        ss >> xc[index];
        ss >> yc[index];
        ss >> zc[index];

        index++;
        assert(index <= nodeList.size());
    }

    xcoord->setData((char*)xc, sizeof(double) * nodeList.size());
    ycoord->setData((char*)yc, sizeof(double) * nodeList.size());
    zcoord->setData((char*)zc, sizeof(double) * nodeList.size());

    xcoord->StoreToDataBase();
    ycoord->StoreToDataBase();
    zcoord->StoreToDataBase();

    delete[] xc; xc = nullptr;
    delete[] yc; yc = nullptr;
    delete[] zc; zc = nullptr;

    int* c = new int[cellListSize];

    index = 0;
    for (auto cell : cellList)
    {
        c[index++] = numberNodes;

        for (size_t nodeIndex = 0; nodeIndex < numberNodes; nodeIndex++)
        {
            c[index++] = (int)cell[nodeIndex];
        }

        c[index++] = cellType;

        assert(index <= cellListSize);
    }

    cells->setData((char*)c, sizeof(int) * cellListSize);
    cells->StoreToDataBase();

    delete[] c; c = nullptr;

    long long xcEntityID        = xcoord->getEntityID();
    long long xcEntityVersion   = xcoord->getEntityStorageVersion();
    long long ycEntityID        = ycoord->getEntityID();
    long long ycEntityVersion   = ycoord->getEntityStorageVersion();
    long long zcEntityID        = zcoord->getEntityID();
    long long zcEntityVersion   = zcoord->getEntityStorageVersion();
    long long cellEntityID      = cells->getEntityID();
    long long cellEntityVersion = cells->getEntityStorageVersion();

    EntityResultUnstructuredMesh* visualizationMesh = new EntityResultUnstructuredMesh(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);;

    visualizationMesh->setMeshData(nodeList.size(), cellList.size(), cellListSize, xcoord, ycoord, zcoord, cells);

    visualizationMesh->StoreToDataBase();

    app->modelComponent()->addNewDataEntity(xcEntityID, xcEntityVersion, visualizationMesh->getEntityID());
    app->modelComponent()->addNewDataEntity(ycEntityID, ycEntityVersion, visualizationMesh->getEntityID());
    app->modelComponent()->addNewDataEntity(zcEntityID, zcEntityVersion, visualizationMesh->getEntityID());
    app->modelComponent()->addNewDataEntity(cellEntityID, cellEntityVersion, visualizationMesh->getEntityID());

    app->modelComponent()->addNewDataEntity(visualizationMesh->getEntityID(), visualizationMesh->getEntityStorageVersion(), solverEntity->getEntityID());

    visualizationMeshID      = visualizationMesh->getEntityID();
    visualizationMeshVersion = visualizationMesh->getEntityStorageVersion();

    delete visualizationMesh;
    visualizationMesh = nullptr;
}

void SolverElectrostatics::storeMeshScalarData(size_t numberPoints, size_t numberCells, std::list<std::string>& potentialList, Application* app, EntityBase* solverEntity, long long& visualizationMeshDataID, long long& visualizationMeshDataVersion)
{
    // Create the potential data item 
    EntityBinaryData* pointScalar = new EntityBinaryData(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
    EntityBinaryData* pointVector = nullptr;
    EntityBinaryData* cellScalar = nullptr;
    EntityBinaryData* cellVector = nullptr;

    float* p = new float[potentialList.size()];

    size_t index = 0;
    for (auto potential : potentialList)
    {
        p[index] = (float)atof(potential.c_str());

        index++;
        assert(index <= potentialList.size());
    }

    pointScalar->setData((char*)p, sizeof(float) * potentialList.size());
    pointScalar->StoreToDataBase();

    delete[] p; p = nullptr;

    long long pointScalarID      = pointScalar->getEntityID();
    long long pointScalarVersion = pointScalar->getEntityStorageVersion();

    EntityResultUnstructuredMeshData* visualizationMeshData = new EntityResultUnstructuredMeshData(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);;

    visualizationMeshData->setData(numberPoints, numberCells, pointScalar, pointVector, cellScalar, cellVector);
    visualizationMeshData->StoreToDataBase();

    app->modelComponent()->addNewDataEntity(pointScalarID, pointScalarVersion, visualizationMeshData->getEntityID());
    app->modelComponent()->addNewDataEntity(visualizationMeshData->getEntityID(), visualizationMeshData->getEntityStorageVersion(), solverEntity->getEntityID());

    visualizationMeshDataID      = visualizationMeshData->getEntityID();
    visualizationMeshDataVersion = visualizationMeshData->getEntityStorageVersion();

    delete visualizationMeshData;
    visualizationMeshData = nullptr;
}

void SolverElectrostatics::storeMeshVectorData(size_t numberPoints, size_t numberCells, std::list<double> &magnitudeList, std::list<std::string> &vectorList, Application* app, EntityBase* solverEntity, long long& visualizationMeshDataID, long long& visualizationMeshDataVersion)
{
    // Create the potential data item 
    EntityBinaryData* pointScalar = nullptr;
    EntityBinaryData* pointVector = nullptr;
    EntityBinaryData* cellScalar = new EntityBinaryData(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
    EntityBinaryData* cellVector = new EntityBinaryData(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);

    float* m = new float[magnitudeList.size()];

    size_t index = 0;
    for (auto mag : magnitudeList)
    {
        m[index] = (float)mag;

        index++;
        assert(index <= magnitudeList.size());
    }

    cellScalar->setData((char*)m, sizeof(float) * magnitudeList.size());
    cellScalar->StoreToDataBase();

    delete[] m; m = nullptr;

    float* v = new float[3 * vectorList.size()];

    index = 0;
    for (auto vec : vectorList)
    {
        std::stringstream ss(vec);

        double vx = 0.0, vy = 0.0, vz = 0.0;

        ss >> vx;
        ss >> vy;
        ss >> vz;

        v[index++] = (float)vx;
        v[index++] = (float)vy;
        v[index++] = (float)vz;

        assert(index <= 3 * vectorList.size());
    }

    cellVector->setData((char*)v, sizeof(float) * 3 * vectorList.size());
    cellVector->StoreToDataBase();

    delete[] v; v = nullptr;

    long long cellScalarID      = cellScalar->getEntityID();
    long long cellScalarVersion = cellScalar->getEntityStorageVersion();
    long long cellVectorID      = cellVector->getEntityID();
    long long cellVectorVersion = cellVector->getEntityStorageVersion();

    EntityResultUnstructuredMeshData* visualizationMeshData = new EntityResultUnstructuredMeshData(app->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);;

    visualizationMeshData->setData(numberPoints, numberCells, pointScalar, pointVector, cellScalar, cellVector);
    visualizationMeshData->StoreToDataBase();

    app->modelComponent()->addNewDataEntity(cellScalarID, cellScalarVersion, visualizationMeshData->getEntityID());
    app->modelComponent()->addNewDataEntity(cellVectorID, cellVectorVersion, visualizationMeshData->getEntityID());
    app->modelComponent()->addNewDataEntity(visualizationMeshData->getEntityID(), visualizationMeshData->getEntityStorageVersion(), solverEntity->getEntityID());

    visualizationMeshDataID = visualizationMeshData->getEntityID();
    visualizationMeshDataVersion = visualizationMeshData->getEntityStorageVersion();

    delete visualizationMeshData;
    visualizationMeshData = nullptr;
}
