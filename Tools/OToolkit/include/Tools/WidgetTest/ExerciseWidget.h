
#pragma once

// Qt header
#include <QtCore/qobject.h>
#include <QtMultimedia/qsoundeffect.h>

#include "OTWidgets/Widgets/RandomizerWheel.h"

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
	void slotMultiFinished(ot::RandomizerWheelItem* _item);
	void slotExercise1Finished(ot::RandomizerWheelItem* _item);
	void slotExercise2Finished(ot::RandomizerWheelItem* _item);
	void slotStart();
	void slotTick1();
	void slotTick2();
	void slotTick3();

private:
	enum class State
	{
		None = 0 << 0,
		IsRunning = 1 << 0,
		WaitForMulti = 1 << 1,
		WaitForExecise1 = 1 << 2,
		WaitForExecise2 = 1 << 3
	};
	typedef ot::Flags<State> StateFlags;
	OT_ADD_FRIEND_FLAG_FUNCTIONS(State, StateFlags)

	StateFlags m_state;

	QWidget* m_root;
	ot::RandomizerWheel* m_multi;
	ot::RandomizerWheel* m_exc;
	ot::Label* m_exc2Label;
	ot::RandomizerWheel* m_exc2;
	QPushButton* m_btn;
	QSoundEffect m_winnerEffect;
	QSoundEffect m_tickEffect1;
	QSoundEffect m_tickEffect2;
	QSoundEffect m_tickEffect3;

	OptionList getMultiplierOptions() const;

	OptionList getExerciseOptions() const;

	void hideExercises();
	void showAndSpinExercises(int _n);

	void checkForCompletion();
};
