#pragma once

// OpenTwin header
#include <openTwin/BlockEditorSharedDatatypes.h>

// Qt header
#include <QtCore/qobject.h>

class BlockConfig;

class QGraphicsItem;
class QSplitter;
class QTreeWidget;
class QTreeWidgetItem;
class QGraphicsScene;
class QGraphicsView;

namespace ot {
	class GraphicItemPickerWidget : public QObject {
		Q_OBJECT
	public:
		GraphicItemPickerWidget();
		virtual ~GraphicItemPickerWidget();

		QWidget* widget(void);

	private:
		QSplitter*      m_splitter;
		QTreeWidget*    m_navigation;
		QGraphicsView*  m_preview;
		QGraphicsScene* m_previewScene;
	};
}