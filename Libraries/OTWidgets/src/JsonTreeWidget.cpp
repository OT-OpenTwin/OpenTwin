// @otlicense

// OpenTwin header
#include "OTWidgets/JsonTreeWidget.h"

// Qt header
#include <QtWidgets/qheaderview.h>

ot::JsonTreeWidget::JsonTreeWidget(QWidget* _parent)
    : QTreeView(_parent), m_readOnly(false)
{
    m_model = new JsonTreeWidgetModel(this);
    setModel(m_model);

    setUniformRowHeights(true);
    setExpandsOnDoubleClick(true);
    
    header()->setStretchLastSection(true);

	connect(this, &QTreeView::expanded, this, &JsonTreeWidget::slotItemExpanded);
}

ot::JsonTreeWidget::~JsonTreeWidget() {
    if (m_model) {
        delete m_model;
        m_model = nullptr;
    }
}

void ot::JsonTreeWidget::setJsonDocument(const QJsonDocument& _doc) {
    OTAssertNullptr(m_model);
    m_model->setJson(_doc);
    expandToDepth(0);
	header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	header()->setSectionResizeMode(0, QHeaderView::Interactive);
}

QJsonDocument ot::JsonTreeWidget::toJsonDocument() const {
    OTAssertNullptr(m_model);
    return m_model->toJsonDocument();
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

void ot::JsonTreeWidget::slotItemExpanded(const QModelIndex& _index) {
    OTAssertNullptr(m_model);
	OTAssert(_index.isValid(), "Invalid index");
    JsonTreeWidgetNode* node = static_cast<JsonTreeWidgetNode*>(_index.internalPointer());
    OTAssertNullptr(node);
    if (!node->isContainer() && (node->getValue().isObject() || node->getValue().isArray())) {
        for (auto child : node->getChildren()) {
            QModelIndex ix = m_model->index(child->getRow(), 0, _index);
			if (ix.isValid()) {
                expand(ix);
			}
		}
    }
}
