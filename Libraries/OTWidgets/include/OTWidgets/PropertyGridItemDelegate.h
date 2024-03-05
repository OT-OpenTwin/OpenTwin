//! @file PropertyGridItemDelegate.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include "QtWidgets/qstyleditemdelegate.h"

namespace ot {

	class PropertyGridItemDelegate : public QStyledItemDelegate
	{
	public:
		PropertyGridItemDelegate(QObject* parent = nullptr);

		void paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;
		
		void setFirstTabSpace(int _space) { m_firstTabSpace = _space; };
		int firstTabSpace(void) const { return m_firstTabSpace; };

	private:
		int m_firstTabSpace;
	};

}