#include <cmath>
#include <cstddef>
#include <list>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>
#include "OTGui/Graphics/Builder/GraphicsHierarchicalItemBuilder.h"

#include "OTServiceFoundation/TransactionData/TransactionEntry.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTGui/Graphics/GraphicsConnectionCfg.h"
#include "OTGui/Graphics/GraphicsItemCfg.h"
#include "OTGui/Graphics/GraphicsPackage.h"


class DependencyGraph
{
public:
    enum class DistributionStyle
    {
        //tree,
        circular
    };

    void createGraph(std::list<ot::TransactionEntry>& _transactions, ot::UID _centerItem, int _distance, DistributionStyle _style);

private:
    struct Edge
    {
        ot::UID from;
        ot::UID to;
    };

    const std::string m_sceneName = "Dependency Graph";

    ot::GraphicsConnectionCfg createEdge(ot::UID _origin, const std::string& _originConnector, ot::UID _destination, const std::string& _destinationConnector);
    ot::GraphicsItemCfg* createNode(ot::UID _origin, const ot::Point2DD& _pos);
    std::string connectorFromDirection(double _dx, double _dy);

    void sendGraphVisualisationRequest(const ot::GraphicsConnectionPackage& _connPckg, const ot::GraphicsScenePackage& _pckg);
};
    