#include "DependencyGraph.h"
#include "OTCore/Geometry/Point2D.h"
#include "OTCore/Logging/Logger.h"

#include "Application.h"

void DependencyGraph::createGraph(std::list<ot::TransactionEntry>& _transactions, ot::UID _centerItem, int _distance, DistributionStyle _style)
{
    if (_distance <= 0)
    {
        throw std::invalid_argument("The node distance must be greater than zero.");
    }

    std::vector<Edge> edges;
    std::set<ot::UID> allNodeIDs;
    std::map<ot::UID, std::set<ot::UID>> outgoing;
    std::map<ot::UID, std::set<ot::UID>> incoming;

    allNodeIDs.insert(_centerItem);

    // Expand each many-to-many TransactionEntry into unique directed edges.
    for (const ot::TransactionEntry& entry : _transactions)
    {
        const std::list<ot::EntityIdentifier>& from = entry.getEntityIdentifierFrom();
        const std::list<ot::EntityIdentifier>& to = entry.getEntityIdentifierTo();

        for (const ot::EntityIdentifier& origin : from)
        {
            allNodeIDs.insert(origin.m_id);

            for (const ot::EntityIdentifier& target : to)
            {
                allNodeIDs.insert(target.m_id);

                if (outgoing[origin.m_id].insert(target.m_id).second)
                {
                    incoming[target.m_id].insert(origin.m_id);
                    edges.push_back({ origin.m_id, target.m_id });
                }
            }
        }
    }

    std::map<ot::UID, ot::Point2DD> positions;
    positions.emplace(_centerItem, ot::Point2DD(0.0, 0.0));

    constexpr double pi = 3.14159265358979323846;
    constexpr double twoPi = 2.0 * pi;

    // Place direct neighbors of the center evenly around the center.
    std::set<ot::UID> centerNeighbors;

    for (const Edge& edge : edges)
    {
        if (edge.from == _centerItem && edge.to != _centerItem)
        {
            centerNeighbors.insert(edge.to);
        }

        if (edge.to == _centerItem && edge.from != _centerItem)
        {
            centerNeighbors.insert(edge.from);
        }
    }

    std::size_t centerNeighborIndex = 0;
    const std::size_t centerNeighborCount = centerNeighbors.size();

    for (const ot::UID neighborID : centerNeighbors)
    {
        const double angle =
            twoPi * static_cast<double>(centerNeighborIndex) /
            static_cast<double>(centerNeighborCount);

        positions.emplace(
            neighborID,
            ot::Point2DD(
                static_cast<double>(_distance) * std::cos(angle),
                static_cast<double>(_distance) * std::sin(angle)));

        ++centerNeighborIndex;
    }

    // Breadth-first placement of nodes reachable from the center.
    std::vector<ot::UID> queue;

    for (const auto& [nodeID, position] : positions)
    {
        queue.push_back(nodeID);
    }

    std::size_t queueIndex = 0;

    while (queueIndex < queue.size())
    {
        const ot::UID parentID = queue[queueIndex++];
        const ot::Point2DD parentPosition = positions.at(parentID);

        std::vector<ot::UID> children;

        for (const ot::UID childID : outgoing[parentID])
        {
            if (positions.find(childID) == positions.end())
            {
                children.push_back(childID);
            }
        }

        if (children.empty())
        {
            continue;
        }

        double directionX = 1.0;
        double directionY = 0.0;

        // Continue away from the first positioned predecessor.
        const auto predecessorList = incoming.find(parentID);

        if (predecessorList != incoming.end())
        {
            for (const ot::UID predecessorID : predecessorList->second)
            {
                const auto predecessorPosition = positions.find(predecessorID);

                if (predecessorPosition != positions.end())
                {
                    directionX =
                        parentPosition.getX() -
                        predecessorPosition->second.getX();

                    directionY =
                        parentPosition.getY() -
                        predecessorPosition->second.getY();
                    break;
                }
            }
        }

        const double directionLength =
            std::sqrt(directionX * directionX + directionY * directionY);

        if (directionLength > 0.0)
        {
            directionX /= directionLength;
            directionY /= directionLength;
        }

        const double baseAngle = std::atan2(directionY, directionX);
        constexpr double fanAngle = pi / 2.0;

        for (std::size_t childIndex = 0;
            childIndex < children.size();
            ++childIndex)
        {
            double relativeAngle = 0.0;

            if (children.size() > 1)
            {
                const double interpolation =
                    static_cast<double>(childIndex) /
                    static_cast<double>(children.size() - 1);

                relativeAngle =
                    -fanAngle / 2.0 + fanAngle * interpolation;
            }

            const double childAngle = baseAngle + relativeAngle;
            const ot::UID childID = children[childIndex];

            positions.emplace(
                childID,
                ot::Point2DD(
                    parentPosition.getX() +
                    static_cast<double>(_distance) *
                    std::cos(childAngle),
                    parentPosition.getY() +
                    static_cast<double>(_distance) *
                    std::sin(childAngle)));

            queue.push_back(childID);
        }
    }

    // Report and ignore every node that was not reachable from the center.
    for (const ot::UID nodeID : allNodeIDs)
    {
        if (positions.find(nodeID) == positions.end())
        {
            OT_LOG_E(
                "Ignoring disconnected or unreachable node with ID: " +
                std::to_string(nodeID));
        }
    }

    
    ot::GraphicsConnectionPackage connPkg(m_sceneName);
    // Create only edges whose origin and target are reachable and positioned.
    for (const Edge& edge : edges)
    {
        const auto originPosition = positions.find(edge.from);
        const auto targetPosition = positions.find(edge.to);

        if (originPosition == positions.end() ||
            targetPosition == positions.end())
        {
            continue;
        }

        const double deltaX = targetPosition->second.getX() - originPosition->second.getX();
        const double deltaY = targetPosition->second.getY() - originPosition->second.getY();

        const std::string originConnector = connectorFromDirection(deltaX, deltaY);
        const std::string targetConnector = connectorFromDirection(-deltaX, -deltaY);
        
        ot::GraphicsConnectionCfg connectionCfg = createEdge(
            edge.from,
            originConnector,
            edge.to,
            targetConnector);
        connPkg.addConnection(connectionCfg);
    }
    
    
    ot::GraphicsScenePackage pckg(m_sceneName);
    // Create only nodes reachable from the center.
    for (const auto& [nodeID, position] : positions)
    {
        ot::GraphicsItemCfg* item = createNode(nodeID, position);
        pckg.addItem(item);
    }

    sendGraphVisualisationRequest(connPkg, pckg);
}

ot::GraphicsConnectionCfg DependencyGraph::createEdge(ot::UID _origin, const std::string& _originConnector, ot::UID _destination, const std::string& _destinationConnector)
{
    ot::GraphicsConnectionCfg edge(_origin, _originConnector, _destination, _destinationConnector);
    edge.setLinePainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemConnection));
    edge.setUid(Application::instance()->getModel()->createEntityUID());
    
    ot::GraphicsTriangleItemCfg* arrowHead = new ot::GraphicsTriangleItemCfg();
    edge.setDestinationLineTip(arrowHead);
    arrowHead->setSize(ot::Size2DD(10., 10.));
    arrowHead->setOutline(ot::PenFCfg(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemConnection)));
    arrowHead->setBackgroundPainer(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemConnection));

    return edge;
}

ot::GraphicsItemCfg* DependencyGraph::createNode(ot::UID _origin, const ot::Point2DD& _pos)
{
    ot::GraphicsHierarchicalItemBuilder builder;
    EntityBase* baseEnt = Application::instance()->getModel()->getEntityByID(_origin);
    builder.setEntityName(baseEnt->getName());
    builder.setTopText(baseEnt->getName());
    builder.setBackgroundShape(ot::GraphicsHierarchicalItemBuilder::BackgroundShape::Ellipse);
    ot::GraphicsItemCfg* node = builder.createGraphicsItem();
    node->setPosition(_pos);
    node->setUid(_origin);
    return node;
}

std::string DependencyGraph::connectorFromDirection(double _dx, double _dy)
{
    ot::Alignment selectedAlignment;
    
    if (std::abs(_dx) >= std::abs(_dy))
    {
        selectedAlignment = _dx >= 0.0 ? ot::Alignment::Right : ot::Alignment::Left;
    }
    else
    {
        selectedAlignment = _dy >= 0.0 ? ot::Alignment::Bottom : ot::Alignment::Top;
    }
    return ot::GraphicsHierarchicalItemBuilder::createConnectorItemName(selectedAlignment);
}
void DependencyGraph::sendGraphVisualisationRequest(const ot::GraphicsConnectionPackage& _connPckg, const ot::GraphicsScenePackage& _pckg)
{
    ot::JsonDocument reqDoc;
    reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, reqDoc.GetAllocator()), reqDoc.GetAllocator());
    ot::VisualisationCfg visualisationCfg;
    ot::JsonObject visualisationCfgJson;
    visualisationCfg.addToJsonObject(visualisationCfgJson, reqDoc.GetAllocator());
    reqDoc.AddMember(OT_ACTION_PARAM_VisualisationConfig, visualisationCfgJson, reqDoc.GetAllocator());

    ot::JsonObject pckgObj;
    _pckg.addToJsonObject(pckgObj, reqDoc.GetAllocator());
    reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, reqDoc.GetAllocator());

    Application::instance()->queuedRequestToFrontend(reqDoc);


    ot::JsonDocument connReqDoc;
    connReqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, reqDoc.GetAllocator()), reqDoc.GetAllocator());
    ot::VisualisationCfg v;
    ot::JsonObject vObj;
    v.addToJsonObject(vObj, connReqDoc.GetAllocator());
    connReqDoc.AddMember(OT_ACTION_PARAM_VisualisationConfig, vObj, connReqDoc.GetAllocator());

    ot::JsonObject connPckgObj;
    _connPckg.addToJsonObject(connPckgObj, connReqDoc.GetAllocator());
    connReqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, connPckgObj, reqDoc.GetAllocator());

    Application::instance()->queuedRequestToFrontend(connReqDoc);
}

