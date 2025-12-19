// @otlicense

// OpenTwin header
#include "OTWidgets/JsonTreeWidget.h"

// Qt header
#include <QtWidgets/qheaderview.h>

ot::JsonTreeWidget::JsonTreeWidget(QWidget* _parent)
    : QTreeView(_parent), m_readOnly(false)
{
    m_model = new JsonTreeWidgetModel(this);
	m_filterModel = new JsonTreeWidgetFilterModel(this);
	m_filterModel->setSourceModel(m_model);
    setModel(m_filterModel);

    setUniformRowHeights(true);
    setExpandsOnDoubleClick(true);
    
    header()->setStretchLastSection(true);

	connect(this, &QTreeView::expanded, this, &JsonTreeWidget::slotItemExpanded);
	connect(this, &QTreeView::doubleClicked, this, &JsonTreeWidget::slotDoubleClicked);
}

ot::JsonTreeWidget::~JsonTreeWidget() {
    if (m_model) {
        delete m_model;
        m_model = nullptr;
    }
}

void ot::JsonTreeWidget::setFromJsonDocument(const JsonDocument& _doc) {
    OTAssertNullptr(m_model);
    
    collapseAll();
    setModel(nullptr);

    m_model->setFromJsonDocument(_doc);

    setModel(m_model);

    expandToDepth(0);
    resizeColumnToContents(JsonTreeWidgetModel::ColumnKey);
}

ot::JsonDocument ot::JsonTreeWidget::toJsonDocument() const {
    OTAssertNullptr(m_model);
    //return m_model->toJsonDocument();
    return JsonDocument();
}

void ot::JsonTreeWidget::filterItems(const QString& _filterText) {
    OTAssertNullptr(m_model);
    m_filterModel->setFilterText(_filterText);

    if (_filterText.isEmpty()) {
        // Show full tree again
        expandToDepth(0);
    }
    else {
        expandAll();
    }
}

bool ot::JsonTreeWidget::edit(const QModelIndex& _index, QAbstractItemView::EditTrigger _trigger, QEvent* _event) {
    if (m_readOnly || !_index.isValid()) {
        return false;
	}
    JsonTreeWidgetNode* node = static_cast<JsonTreeWidgetNode*>(_index.internalPointer());
    OTAssertNullptr(node);
    if (node->isEditProtected()) {
        return false;
	}
    else {
        return QTreeView::edit(_index, _trigger, _event);
    }
}

void ot::JsonTreeWidget::slotDoubleClicked(const QModelIndex& _index) {
    OTAssertNullptr(m_model);
    OTAssert(_index.isValid(), "Invalid index");
    JsonTreeWidgetNode* node = static_cast<JsonTreeWidgetNode*>(_index.internalPointer());
    OTAssertNullptr(node);
	Q_EMIT nodeDoubleClicked(_index.column(), node);
}

void ot::JsonTreeWidget::slotItemExpanded(const QModelIndex& _index) {
    OTAssertNullptr(m_model);
	OTAssert(_index.isValid(), "Invalid index");

    // Check if first column was expanded
    if (_index.column() == JsonTreeWidgetModel::ColumnKey) {
        JsonTreeWidgetNode* node = static_cast<JsonTreeWidgetNode*>(_index.internalPointer());
        OTAssertNullptr(node);

		// Ensure node is object/array and not a child container of those
        if (!node->isContainer() && (node->getType() == JsonTreeWidgetNode::Object || node->getType() == JsonTreeWidgetNode::Array)) {
			// Iterate over all children and expand them as well
            for (auto child : node->getChildren()) {
                QModelIndex ix = m_model->index(child->getRow(), 0, _index);
                if (ix.isValid()) {
                    expand(ix);
                }
            }
        }
        
		auto initialSize = header()->sectionSize(JsonTreeWidgetModel::ColumnKey);
		resizeColumnToContents(JsonTreeWidgetModel::ColumnKey);
        if (header()->sectionSize(JsonTreeWidgetModel::ColumnKey) < initialSize) {
            header()->resizeSection(JsonTreeWidgetModel::ColumnKey, initialSize);
        }
    }
}
