#include "SolverFDTD.h"
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

void SolverFDTD::writeInputFile(std::ofstream& _controlFile, Application *app)
{
    // Get map of all materials and their corresponding objects
    std::map<std::string, std::list<std::string>> materialsToObjectsMap;
    getMaterialsToObjectsMap(materialsToObjectsMap, app);

    // Get map of all shapes with potential definition
    std::map<std::string, double> boundaryCondition;
    getBoundaryConditions(boundaryCondition);

    // Now we build (alias) names for each material
    std::map<std::string, std::string> materialNameToAliasMap;
    buildMaterialAliases(materialsToObjectsMap, materialNameToAliasMap);

    // Now we build (alias) names for each potential definition
    std::map<std::string, std::string> boundaryConditionsNameToAliases;
    buildBoundaryConditionAliases(boundaryCondition, boundaryConditionsNameToAliases);

    // Write the groups defining the regions
    //writeGroups(_controlFile, materialsToObjectsMap, materialNameToAliasMap, potentialDefinitions, potentialNameToAliasMap);

    // Write the functions defining the materials
    writeFunctions(_controlFile, materialNameToAliasMap);

    // Write the constraints defining the boundary conditions and potentials
    writeConstraints(_controlFile, boundaryCondition, boundaryConditionsNameToAliases);

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

std::string SolverFDTD::runSolver(const std::string& tempDirPath, ot::components::UiComponent* uiComponent)
{
    runSolverExe("model", "FDTD_XML", "Map", tempDirPath, uiComponent);

    return solverOutput.str();
}

void SolverFDTD::getBoundaryConditions(std::map<std::string, double>& boundaryConditionDefinitions) {

    for (auto item : entityProperties) {
        EntityPropertiesDouble* boundaryCondition = dynamic_cast<EntityPropertiesDouble*>(item.second.getProperty("Boundary Condition"));

        if (boundaryCondition != nullptr) {
            boundaryConditionDefinitions[meshItemInfo[item.first].getEntityName()] = boundaryCondition->getValue();
        }
    }
}

void SolverFDTD::getMaterialsToObjectsMap(std::map<std::string, std::list<std::string>>& materialsToObjectsMap, Application* app)
{
    // Here we need to loop through all mesh items and their properties and get their materials
    for (auto item : entityProperties)
    {
        if (meshItemInfo[item.first].getEntityType() == "EntityMeshTetItem")
        {
            EntityPropertiesEntityList* materialProperty = dynamic_cast<EntityPropertiesEntityList*>(item.second.getProperty("Material"));

            if (materialProperty != nullptr)
            {
                std::string materialName = app->modelComponent()->getCurrentMaterialName(materialProperty);
                materialsToObjectsMap[materialName].push_back(meshItemInfo[item.first].getEntityName());
            }
        }
    }
}

void SolverFDTD::buildMaterialAliases(std::map<std::string, std::list<std::string>>& materialsToObjectsMap, std::map<std::string, std::string>& materialNameToAliasMap)
{
    // Now we create an alias name "material#n" for each material
    int count = 1;
    for (auto material : materialsToObjectsMap)
    {
        materialNameToAliasMap[material.first] = "material" + std::to_string(count);
        count++;
    }
}

void SolverFDTD::buildBoundaryConditionAliases(std::map<std::string, double>& boundaryConditionDefinitions, std::map<std::string, std::string>& boundaryConditionsNameToAliasMap) {
    
    int count = 1;

    for (auto material : boundaryConditionDefinitions) {
        boundaryConditionsNameToAliasMap[material.first] = "boundary" + std::to_string(count);
        count++;
    }
}

void SolverFDTD::writeGroups(std::ofstream& controlFile, 
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

std::list<int> SolverFDTD::meshVolumeGroupIdList(const std::list<std::string>& itemNames)
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

std::list<int> SolverFDTD::meshSurfaceGroupIdList(const std::list<std::string>& itemNames)
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

std::string SolverFDTD::getGroupList(const std::list<int>& list)
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

std::string SolverFDTD::getGroupList(const std::list<std::string>& list)
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

void SolverFDTD::writeFunctions(std::ofstream& controlFile, std::map<std::string, std::string>& materialNameToAliasMap)
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

void SolverFDTD::writeConstraints(std::ofstream& controlFile, std::map<std::string, double>& boundaryConditionDefinition, std::map<std::string, std::string>& boundaryConditionNameToAliasMap)
{
    controlFile <<
        "Constraint {\n"
        "  { Name Boundary Conditions; Type Assign;\n"
        "    Case {\n";

    for (auto boundaryCondition : boundaryConditionDefinition)
    {
        std::string boundaryConditionName = boundaryConditionNameToAliasMap[boundaryCondition.first];
        std::list<int> groupItemList = meshSurfaceGroupIdList(std::list<std::string>{boundaryCondition.first});
        std::string groupList = getGroupList(groupItemList);

        controlFile << "      { Region " << boundaryConditionName << "; Value " << boundaryCondition.second << "; }\n";
    }

    controlFile <<
        "      { Region SurfInf; Value 0.0; }\n"
        "    }\n"
        "  }\n";

    controlFile <<
        "  { Name Boundary Conditions_d2; Type Assign;\n"
        "    Case {\n";

    for (auto boundaryCondition : boundaryConditionDefinition)
    {
        std::string boundaryConditionName = boundaryConditionNameToAliasMap[boundaryCondition.first];
        std::list<int> groupItemList = meshSurfaceGroupIdList(std::list<std::string>{boundaryCondition.first});
        std::string groupList = getGroupList(groupItemList);

        controlFile << "      { Region " << boundaryConditionName << "; Value 0.0; }\n";
    }

    controlFile <<
        "      { Region SurfInf; Value 0.0; }\n"
        "    }\n"
        "  }\n"
        "}\n\n";
}

void SolverFDTD::writeFunctionSpace(std::ofstream& controlFile)
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

void SolverFDTD::writeJacobian(std::ofstream& controlFile)
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

void SolverFDTD::writeIntegration(std::ofstream& controlFile)
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

void SolverFDTD::writeFormulation(std::ofstream& controlFile)
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

void SolverFDTD::writeResolution(std::ofstream& controlFile)
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

void SolverFDTD::writePostProcessing(std::ofstream& controlFile)
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

void SolverFDTD::writePostOperation(std::ofstream& controlFile)
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

size_t SolverFDTD::getOrAddNode(const std::string& node, const std::string& potential, 
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

size_t SolverFDTD::getOrAddCellNode(const std::string& node, std::map<std::string, size_t>& nodeToIndexMap, std::list<std::string>& nodeList, size_t& nodeIndex)
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

void SolverFDTD::storeMesh(int numberNodes, int cellType, std::list<std::string>& nodeList, std::list<std::vector<size_t>>& cellList, size_t& cellListSize, Application* app, EntityBase* solverEntity, long long& visualizationMeshID, long long& visualizationMeshVersion)
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

void SolverFDTD::storeMeshScalarData(size_t numberPoints, size_t numberCells, std::list<std::string>& potentialList, Application* app, EntityBase* solverEntity, long long& visualizationMeshDataID, long long& visualizationMeshDataVersion)
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

void SolverFDTD::storeMeshVectorData(size_t numberPoints, size_t numberCells, std::list<double> &magnitudeList, std::list<std::string> &vectorList, Application* app, EntityBase* solverEntity, long long& visualizationMeshDataID, long long& visualizationMeshDataVersion)
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
