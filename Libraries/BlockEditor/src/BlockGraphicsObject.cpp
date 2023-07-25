//! @file BlockGraphicsObject.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockGraphicsObject.h"

void ot::BlockGraphicsObject::setMargins(const ot::MarginsD& _margins) {
	m_margins.setBottom(_margins.bottom());
	m_margins.setLeft(_margins.left());
	m_margins.setRight(_margins.right());
	m_margins.setTop(_margins.top());
}

QSizeF ot::BlockGraphicsObject::applyLimits(const QSizeF& _size) const {
	QSizeF r(_size);

	// Width min
	if (m_widthLimit.isMinSet()) {
		if (m_widthLimit.min() > r.width()) {
			r.setWidth(m_widthLimit.min());
		}
	}

	// Width max
	if (m_widthLimit.isMaxSet()) {
		if (m_widthLimit.max() < r.width()) {
			r.setWidth(m_widthLimit.max());
		}
	}

	// Height min
	if (m_heightLimit.isMinSet()) {
		if (m_heightLimit.min() > r.height()) {
			r.setHeight(m_heightLimit.min());
		}
	}

	// Height max
	if (m_heightLimit.isMaxSet()) {
		if (m_heightLimit.max() < r.height()) {
			r.setHeight(m_heightLimit.max());
		}
	}

	return r;
}

QSizeF ot::BlockGraphicsObject::addMargins(const QSizeF& _size) const {
	return QSizeF(
		_size.width() + m_margins.left() + m_margins.right(), 
		_size.height() + m_margins.top() + m_margins.bottom()
	);
}
