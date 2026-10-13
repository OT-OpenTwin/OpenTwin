
#pragma once

// Qt header
#include <QtCore/qobject.h>
#include <QtMultimedia/qsoundeffect.h>

#include "Tools/WidgetTest/RandomizerWheel.h"

class QPushButton;

namespace ot
{
	class Label;
}

class ExerciseWidget : public QObject
{
public:
	enum class OptionMode
	{
		None,
		ShowExercise2
	};

	typedef std::pair<QString, QVariant> OptionEntry;
	typedef std::vector<OptionEntry> OptionList;

	ExerciseWidget();

	QWidget* getRootWidget() const { return m_root; };

private Q_SLOTS:
	void slotMultiFinished(RandomizerWheelItem* _item);
	void slotExercise1Finished(RandomizerWheelItem* _item);
	void slotExercise2Finished(RandomizerWheelItem* _item);
	void slotStart();

private:
	enum class State
	{
		None            = 0 << 0,
		IsRunning       = 1 << 0,
		WaitForMulti    = 1 << 1,
		WaitForExecise1 = 1 << 2,
		WaitForExecise2 = 1 << 3
	};
	typedef ot::Flags<State> StateFlags;
	OT_ADD_FRIEND_FLAG_FUNCTIONS(State, StateFlags)

	StateFlags m_state;

	QWidget* m_root;
	RandomizerWheel* m_multi;
	RandomizerWheel* m_exc;
	ot::Label* m_exc2Label;
	RandomizerWheel* m_exc2;
	QPushButton* m_btn;
	QSoundEffect m_winnerEffect;

	OptionList getMultiplierOptions() const;

	OptionList getExerciseOptions() const;

	void hideExercises();
	void showAndSpinExercises(int _n);

	void checkForCompletion();
};
