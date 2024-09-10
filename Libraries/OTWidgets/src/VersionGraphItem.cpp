//! @file VersionGraphItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/StyleRefPainter2D.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/VersionGraphItem.h"
#include "OTWidgets/GraphicsTextItem.h"
#include "OTWidgets/GraphicsGridLayoutItem.h"
#include "OTWidgets/GraphicsRectangularItem.h"
#include "OTWidgets/GraphicsInvisibleItem.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/GraphicsLayoutItemWrapper.h"

#define OT_VERSIONGRAPHITEM_Height 50.
#define OT_VERSIONGRAPHITEM_VSpacing 30.
#define OT_VERSIONGRAPHITEM_HSpacing 60.

ot::VersionGraphItem::VersionGraphItem() 
	: m_parentVersion(nullptr), m_parentConnection(nullptr), m_row(0)
{
	this->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsState);
	
	GraphicsGridLayoutItem* centralLayout = new GraphicsGridLayoutItem;
	centralLayout->setGraphicsItemName("CentralLayout");
	centralLayout->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemForwardsState);
	centralLayout->setGraphicsItemMinimumSize(100., OT_VERSIONGRAPHITEM_Height);
	centralLayout->setGraphicsItemMaximumSize(std::numeric_limits<double>::max(), OT_VERSIONGRAPHITEM_Height);
	centralLayout->setGraphicsItemSizePolicy(SizePolicy::Preferred);
	centralLayout->getLayoutWrapper()->setMinimumSize(100., OT_VERSIONGRAPHITEM_Height);
	centralLayout->getLayoutWrapper()->setMaximumSize(std::numeric_limits<double>::max(), OT_VERSIONGRAPHITEM_Height);
	centralLayout->setContentsMargins(0., 0., 0., 0.);

	// Create items
	GraphicsRectangularItem* border = new GraphicsRectangularItem;
	border->setGraphicsItemName("Border");
	border->setGraphicsItemFlags(GraphicsItemCfg::ItemHandlesState);
	border->setBackgroundPainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBackground));
	border->setOutline(OutlineF(1., new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBorder)));
	border->setGraphicsItemSizePolicy(SizePolicy::Dynamic);

	m_inConnector = new GraphicsInvisibleItem;
	m_inConnector->setGraphicsItemName("In");
	m_inConnector->setGraphicsItemAlignment(AlignCenter);
	m_inConnector->setGraphicsItemMaximumSize(1., 1.);
	m_inConnector->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
	m_inConnector->setConnectionDirection(ConnectLeft);

	m_outConnector = new GraphicsInvisibleItem;
	m_outConnector->setGraphicsItemName("Out");
	m_outConnector->setGraphicsItemAlignment(AlignCenter);
	m_outConnector->setGraphicsItemMaximumSize(1., 1.);
	m_outConnector->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
	m_outConnector->setConnectionDirection(ConnectRight);

	m_textItem = new GraphicsTextItem;
	m_textItem->setGraphicsItemName("Text");
	m_textItem->setGraphicsItemSizePolicy(SizePolicy::Preferred);
	m_textItem->setGraphicsItemAlignment(AlignCenter);
	m_textItem->setGraphicsItemFlags(GraphicsItemCfg::ItemHandlesState | GraphicsItemCfg::ItemForwardsTooltip);
	m_textItem->setTextPainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemForeground));

	// Setup layouts
	this->addItem(border, false, true);
	this->addItem(centralLayout, true, false);

	centralLayout->addItem(m_inConnector, 1, 0);
	centralLayout->addItem(m_textItem, 1, 1);
	centralLayout->addItem(m_outConnector, 1, 2);

	centralLayout->setRowStretchFactor(0, 1);
	centralLayout->setRowStretchFactor(2, 1);
	centralLayout->setColumnStretchFactor(1, 1);

	centralLayout->setRowAlignment(1, Qt::AlignCenter);
	centralLayout->setColumnAlignment(1, Qt::AlignCenter);

	this->setZValue(2);
}

ot::VersionGraphItem::~VersionGraphItem() {
	if (m_parentVersion) {
		m_parentVersion->forgetChildVersion(this);
		m_parentVersion = nullptr;
	}

	for (VersionGraphItem* child : m_childVersions) {
		child->setParentVersionItem(nullptr);
	}
}

void ot::VersionGraphItem::setVersionConfig(const VersionGraphVersionCfg& _config) {
	OTAssertNullptr(this->getGraphicsScene());

	// Clear data
	std::list<VersionGraphItem*> childs = m_childVersions;
	m_childVersions.clear();
	for (VersionGraphItem* child : childs) {
		child->setParentVersionItem(nullptr);
		delete child;
	}

	// Apply config
	m_config = _config;

	// Create item config
	this->updateGraphics();

	// Create childs
	int childRow = m_row;

	for (const VersionGraphVersionCfg& childCfg : m_config.getChildVersions()) {
		VersionGraphItem* newChild = new VersionGraphItem;
		newChild->setParentVersionItem(this);
		newChild->setRowIndex(childRow);
		
		newChild->setGraphicsScene(this->getGraphicsScene());
		this->getGraphicsScene()->addItem(newChild);

		newChild->setVersionConfig(childCfg);
		newChild->connectToParent();
		
		m_childVersions.push_back(newChild);

		childRow = newChild->getMaxRowIndex() + 1;
	}
}

int ot::VersionGraphItem::getMaxRowIndex(void) const {
	int ix = m_row;
	for (const VersionGraphItem* child : m_childVersions) {
		ix = std::max(ix, child->getMaxRowIndex());
	}
	return ix;
}

void ot::VersionGraphItem::addChildVersion(VersionGraphItem* _version) {
	OTAssert(std::find(m_childVersions.begin(), m_childVersions.end(), _version) != m_childVersions.end(), "Child version already stored");
	m_childVersions.push_back(_version);
}

void ot::VersionGraphItem::forgetChildVersion(VersionGraphItem* _version) {
	auto it = std::find(m_childVersions.begin(), m_childVersions.end(), _version);
	if (it != m_childVersions.end()) {
		m_childVersions.erase(it);
	}
}

void ot::VersionGraphItem::connectToParent(void) {
	if (m_parentConnection) {
		delete m_parentConnection;
	}
	m_parentConnection = new GraphicsConnectionItem;
	GraphicsConnectionCfg connectionConfig;
	connectionConfig.setLinePainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemConnection));
	connectionConfig.setLineShape(GraphicsConnectionCfg::ConnectionShape::SmoothLine);
	connectionConfig.setHandlesState(false);
	m_parentConnection->setConfiguration(connectionConfig);
	m_parentConnection->connectItems(m_parentVersion->m_outConnector, m_inConnector);
	m_parentConnection->setZValue(1);
	OTAssertNullptr(this->getGraphicsScene());
	this->getGraphicsScene()->addItem(m_parentConnection);
}

ot::VersionGraphItem* ot::VersionGraphItem::findVersionByName(const std::string& _versionName) {
	if (_versionName == m_config.getName()) return this;
	VersionGraphItem* result = nullptr;
	for (VersionGraphItem* item : m_childVersions) {
		result = item->findVersionByName(_versionName);
		if (result) break;
	}
	return result;
}

void ot::VersionGraphItem::updateGraphics(void) {
	this->prepareGeometryChange();

	this->setGraphicsItemName(m_config.getName());
	this->setGraphicsItemToolTip(m_config.getDescription());

	// Calculate positioning
	Point2DD newPos(0., (OT_VERSIONGRAPHITEM_Height + OT_VERSIONGRAPHITEM_VSpacing) * (double)m_row);

	if (m_parentVersion) {
		newPos.setX(m_parentVersion->getQGraphicsItem()->pos().x() + m_parentVersion->getQGraphicsItem()->boundingRect().width() + OT_VERSIONGRAPHITEM_HSpacing);
	}
	this->setGraphicsItemPos(newPos);
	
	m_textItem->setText(m_config.getTitle());
}
