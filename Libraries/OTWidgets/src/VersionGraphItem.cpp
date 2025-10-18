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

ot::VersionGraphItem::VersionGraphItem(const VersionGraphVersionCfg& _config, int _row, const std::string& _activeVersion, GraphicsScene* _scene) :
	m_parentVersion(nullptr), m_parentConnection(nullptr), m_row(_row), m_config(_config)
{
	OTAssertNullptr(_scene);
	this->setGraphicsScene(_scene);

	_scene->addItem(this);

	this->setGraphicsItemName(m_config.getName());
	this->setGraphicsItemToolTip(m_config.getDescription());
	this->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsState | GraphicsItemCfg::ItemIsSelectable);
	
	GraphicsGridLayoutItem* centralLayout = new GraphicsGridLayoutItem;
	centralLayout->setGraphicsItemName("CentralLayout");
	centralLayout->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemForwardsState);
	//m_layout->setGraphicsItemMaximumSize(std::numeric_limits<double>::max(), OT_VERSIONGRAPHITEM_Height);
	//m_layout->setGraphicsItemSizePolicy(SizePolicy::Preferred);
	centralLayout->getLayoutWrapper()->setMinimumSize(100., OT_VERSIONGRAPHITEM_Height);
	centralLayout->getLayoutWrapper()->setMaximumHeight(OT_VERSIONGRAPHITEM_Height);
	centralLayout->setContentsMargins(0., 0., 0., 0.);

	// Create items
	GraphicsRectangularItem* border = new GraphicsRectangularItem;
	border->setGraphicsItemName("Border");
	border->setGraphicsItemFlags(GraphicsItemCfg::ItemHandlesState | GraphicsItemCfg::ItemForwardsTooltip);
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

	m_nameItem = new GraphicsTextItem;
	m_nameItem->setGraphicsItemName("Name");
	m_nameItem->setGraphicsItemFlags(GraphicsItemCfg::ItemHandlesState | GraphicsItemCfg::ItemForwardsTooltip);
	m_nameItem->setGraphicsItemMargins(MarginsD(0., 3., 0., 0.));
	
	m_labelItem = new GraphicsTextItem;
	m_labelItem->setGraphicsItemName("Label");
	m_labelItem->setGraphicsItemFlags(GraphicsItemCfg::ItemHandlesState | GraphicsItemCfg::ItemForwardsTooltip);
	
	GraphicsInvisibleItem* spacerItem = new GraphicsInvisibleItem;

	Font textFont = m_nameItem->getFont();
	textFont.setSize(6);
	textFont.setBold(m_config.getName() == _activeVersion);
	m_nameItem->setFont(textFont);
	textFont.setSize(8);
	m_labelItem->setFont(textFont);

	PenFCfg borderOutline(1., new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBorder));
	if (m_config.getName() == _activeVersion) {
		borderOutline.setStyle(LineStyle::DotLine);
		border->setBackgroundPainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemForeground));
		m_labelItem->setTextPainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBackground));
		m_nameItem->setTextPainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBackground));
	}
	else {
		border->setBackgroundPainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBackground));
		m_labelItem->setTextPainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemForeground));
		m_nameItem->setTextPainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemForeground));
	}
	border->setOutline(borderOutline);

	// Setup layouts
	this->addItem(border, false, true);
	this->addItem(centralLayout, true, false);

	centralLayout->addItem(m_inConnector, 1, 0, Qt::AlignVCenter | Qt::AlignLeft);
	centralLayout->addItem(m_nameItem, 0, 1, Qt::AlignLeft);
	centralLayout->addItem(m_labelItem, 1, 1, Qt::AlignCenter);
	centralLayout->addItem(m_outConnector, 1, 2, Qt::AlignVCenter | Qt::AlignRight);
	centralLayout->addItem(spacerItem, 2, 1, Qt::AlignCenter);

	centralLayout->setRowStretchFactor(0, 1);
	centralLayout->setRowStretchFactor(2, 1);
	centralLayout->setColumnStretchFactor(1, 1);

	if (m_config.getLabel().empty()) {
		m_nameItem->setText("");
		m_labelItem->setText(m_config.getName());
	}
	else {
		m_nameItem->setText(m_config.getName());
		m_labelItem->setText(m_config.getLabel());
	}

	this->finalizeGraphicsItem();

	m_currentSize = this->boundingRect().size();
}

ot::VersionGraphItem::~VersionGraphItem() {
	// Erase parent information from childs
	for (VersionGraphItem* child : m_childVersions) {
		child->disconnectFromParent();
		child->setParentVersionItem(nullptr);
	}

	// Forget childs
	m_childVersions.clear();

	// Forget parent connection
	if (m_parentConnection) {
		m_parentConnection->disconnectItems(false);
		delete m_parentConnection;
		m_parentConnection = nullptr;
	}

	// Erase this information from parent
	if (m_parentVersion) {
		m_parentVersion->forgetChildVersion(this);
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
	OTAssert(std::find(m_childVersions.begin(), m_childVersions.end(), _version) == m_childVersions.end(), "Child version already stored");
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
		OT_LOG_WA("Connection already set");
		delete m_parentConnection;
		m_parentConnection = nullptr;
	}
	
	m_parentConnection = new GraphicsConnectionItem;
	GraphicsConnectionCfg connectionConfig;
	connectionConfig.setLinePainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemConnection));
	connectionConfig.setLineShape(GraphicsConnectionCfg::ConnectionShape::SmoothLine);
	connectionConfig.setLineStyle((m_config.getDirectParentIsHidden() ? LineStyle::DashDotDotLine : LineStyle::SolidLine));
	connectionConfig.setLineWidth((m_config.getDirectParentIsHidden() ? .75 : 1.));
	connectionConfig.setHandlesState(false);
	m_parentConnection->setConfiguration(connectionConfig);
	m_parentConnection->connectItems(m_parentVersion->m_outConnector, m_inConnector);
	OTAssertNullptr(this->getGraphicsScene());
	this->getGraphicsScene()->addItem(m_parentConnection);

	// Register at parent
	m_parentVersion->addChildVersion(this);
}

void ot::VersionGraphItem::disconnectFromParent(void) {
	if (m_parentConnection) {
		m_parentConnection->disconnectItems(false);
		delete m_parentConnection;
		m_parentConnection = nullptr;
	}
}

void ot::VersionGraphItem::updateVersionPositionAndSize(void) {
	this->prepareGeometryChange();

	m_currentSize = this->boundingRect().size();

	// Calculate positioning
	QPoint newPos(0., (OT_VERSIONGRAPHITEM_Height + OT_VERSIONGRAPHITEM_VSpacing) * (double)m_row);

	if (m_parentVersion) {
		newPos.setX(
			m_parentVersion->getQGraphicsItem()->pos().x() +
			m_parentVersion->getCurrentSize().width() + OT_VERSIONGRAPHITEM_HSpacing
		);
	}
	this->getQGraphicsItem()->setPos(newPos);

	// Update childs
	for (VersionGraphItem* child : m_childVersions) {
		child->updateVersionPositionAndSize();
	}
}

void ot::VersionGraphItem::setIsInActiveBranch(void) {
	if (m_parentConnection) {
		m_parentConnection->setLinePainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemSelectionBorder));
	}
}

