#pragma once

#include <akTools/ClassHelper.h>
#include <akWidgets/aWidget.h>

#include <qobject.h>

#include <vector>

class QWidget;
class QGridLayout;
class QShortcut;
class QKeyEvent;

namespace ak {
	class aCheckBoxWidget;
	class aLineEditWidget;
	class aChecklistWidget;

	class UICORE_API_EXPORT aChecklistItem : public QObject, public aObject {
		Q_OBJECT
	public:
		aChecklistItem(aChecklistWidget * _owner = (aChecklistWidget *)nullptr);
		aChecklistItem(const QString& _text, bool _isChecked, aChecklistWidget * _owner);
		virtual ~aChecklistItem();

		A_PROPERTY(int, Index);
		A_PROPERTY_POINTER(aCheckBoxWidget *, CheckBox);
		A_PROPERTY_POINTER(aLineEditWidget *, TextEdit);
		A_PROPERTY_POINTER(aChecklistWidget *, ChecklistWidget);

		void setText(const QString& _text);
		QString text(void) const;

		void setChecked(bool _isChecked);
		bool isChecked(void) const;

		void refreshAppearance(void);

		bool checkTextModified(void);

	signals:
		void textChanged(int _index);
		void editingFinished(int _index);
		void returnPressed(int _index);
		void keyUpPressed(int _index);
		void keyDownPressed(int _index);
		void requestDelete(int _index);
		void moveUpRequested(int _index);
		void moveDownRequested(int _index);

	private slots:
		void slotCheckedChanged(int _state);
		void slotTextChanged(void);
		void slotEditingFinished(void);
		void slotToggleChecked(void);
		void slotRequestDelete(void);
		void slotReturnPressed(void);
		void slotKeyEvent(QKeyEvent * _event);
		void slotMoveUpRequested(void);
		void slotMoveDownRequested(void);

	private:

		void ini(void);

		QString			m_text;
		QShortcut *		m_shortcutChangeCheckedState;
		QShortcut *		m_shortcutDeleteItem;
		QShortcut *		m_shortcutDeleteItem2;
		QShortcut *		m_shortcutMoveUp;
		QShortcut *		m_shortcutMoveDown;

		aChecklistItem(aChecklistItem&) = delete;
		aChecklistItem& operator = (aChecklistItem&) = delete;
	};

	// ######################################################################################################################################

	// ######################################################################################################################################

	// ######################################################################################################################################

	class UICORE_API_EXPORT aChecklistWidget : public QObject, public aWidget {
		Q_OBJECT
	public:
		aChecklistWidget();
		virtual ~aChecklistWidget();

		virtual QWidget * widget(void) override;
		
		void setCrossoutCheckedItemsEnabled(bool _enabled);
		bool isCrossoutCheckedItemsEnabled(void) const { return m_crossOutCheckedItems; }
		
		void setPointSize(int _pointSize);
		int pointSize(void) const { return m_pointSize; }

		void refreshAppearance(void);

		void addItem(const QString& _text, bool _isChecked);

		aChecklistItem * item(int _index);
		const std::vector<aChecklistItem *>& items(void) const { return m_items; }

		void clear(bool _addEmptyItem = true);

	signals:
		void itemOrderChanged(void);
		void itemChanged(int _index);
		void itemRemoved(int _index);
		void itemCreated(int _index);

	private slots:
		void slotItemChanged(int _index);
		void slotReturnPressed(int _index);
		void slotFocusNext(int _index);
		void slotFocusPrev(int _index);
		void slotDeleteItem(int _index);
		void slotMoveItemUp(int _index);
		void slotMoveItemDown(int _index);

	private:

		void clearEmptyItems(void);
		void addItem(aChecklistItem * _item);
		void initializeNewItem(aChecklistItem * _item, int _index);
		void switchItemLocations(int _index1, int _index2);
		void moveItemToTheTop(int _index);
		void moveItemToTheBottom(int _index);

		QWidget *						m_centralLayoutW;
		QGridLayout *					m_centralLayout;

		std::vector<aChecklistItem *>	m_items;

		bool							m_crossOutCheckedItems;
		int								m_pointSize;

		aChecklistWidget(aChecklistWidget&) = delete;
		aChecklistWidget& operator = (aChecklistWidget&) = delete;
	};

}
