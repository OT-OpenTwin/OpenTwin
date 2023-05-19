//! @file BlockPickerWidget.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qsize.h>

// std header
#include <list>
#include <map>

class QGraphicsItem;
class QSplitter;
class QTreeWidgetItem;
class QGridLayout;

namespace ot {

	class TreeWidgetFilter;
	class GraphicsView;
	class GraphicsScene;
	class BlockConfiguration;
	class BlockCategoryConfiguration;

	class BLOCK_EDITOR_API_EXPORT BlockPickerWidget : public QObject {
		Q_OBJECT
	public:
		BlockPickerWidget(Qt::Orientation _orientation = Qt::Vertical);
		virtual ~BlockPickerWidget();

		QWidget* widget(void);

		void setOrientation(Qt::Orientation _orientation);
		Qt::Orientation orientation(void) const;

		void addTopLevelCategory(ot::BlockCategoryConfiguration* _topLevelCategory);
		void addTopLevelCategories(const std::list<ot::BlockCategoryConfiguration *>& _topLevelCategories);
		
		void clear(void);

		void setPreviewBoxSize(const QSize& _size) { m_previewSize = _size; };
		const QSize& previewBoxSize(void) const { return m_previewSize; };

	private slots:
		void slotSelectionChanged(void);

	private:
		void addCategory(ot::BlockCategoryConfiguration* _category, QTreeWidgetItem* _parentNavigationItem);
		void addCategories(const std::list<ot::BlockCategoryConfiguration*>& _categories, QTreeWidgetItem* _parentNavigationItem);

		void addBlockToNavigation(ot::BlockConfiguration* _block, QTreeWidgetItem* _parentNavigationItem);
		void addBlocksToNavigation(const std::list <ot::BlockConfiguration*>& _blocks, QTreeWidgetItem* _parentNavigationItem);

		void storePreviewData(QTreeWidgetItem* _item, BlockConfiguration* _config);

		struct PreviewBox {
			GraphicsView* view;
			GraphicsScene* scene;
		};

		bool                  m_repaintPreviewRequired;

		QSize                 m_previewSize;
		QSplitter*            m_splitter;
		TreeWidgetFilter*     m_navigation;
		
		std::list<PreviewBox> m_views;
		QWidget*              m_viewLayoutW;
		QGridLayout *         m_viewLayout;

		std::map<QTreeWidgetItem*, std::list<BlockConfiguration*>> m_previewData;
	};	
}