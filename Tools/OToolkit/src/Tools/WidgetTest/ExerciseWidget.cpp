
#include "Tools/WidgetTest/ExerciseWidget.h"

#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Widgets/Label.h"
#include "OTWidgets/Widgets/PushButton.h"

#include "OToolkitAPI/OToolkitAPI.h"

#include <QtCore/qtimer.h>
#include <QtCore/qrandom.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qslider.h>
#include <QtWidgets/qmessagebox.h>

#define EXERCISE_LOG(___message) OTOOLKIT_LOG("Exercise", ___message)
#define EXERCISE_LOGW(___message) OTOOLKIT_LOGW("Exercise", ___message)
#define EXERCISE_LOGE(___message) OTOOLKIT_LOGE("Exercise", ___message)

ExerciseWidget::ExerciseWidget()
	: m_winnerEffect(this)
{
	m_root = new QWidget;

	const int initialVolume = 25;
	const float initialVolumeF = static_cast<float>(initialVolume) / 100.0f;

	QVBoxLayout* layout = new QVBoxLayout(m_root);
	QHBoxLayout* topLayout = new QHBoxLayout;
	layout->addLayout(topLayout);

	QSlider* volumeSlider = new QSlider(Qt::Horizontal, m_root);
	topLayout->addWidget(volumeSlider);
	QLabel* volumeLabel = new QLabel(QString::number(initialVolume) + "%", m_root);
	topLayout->addWidget(volumeLabel);
	topLayout->addStretch(1);
	volumeSlider->setRange(0, 100);
	volumeSlider->setValue(initialVolume);
	volumeSlider->setTickPosition(QSlider::TicksBelow);
	volumeSlider->setTickInterval(10);
	connect(volumeSlider, &QSlider::valueChanged, this, [this, volumeLabel](int _value) {
		const float volumeF = static_cast<float>(_value) / 100.0f;
		m_winnerEffect.setVolume(volumeF);
		m_multi->setTickVolume(volumeF);
		m_exc->setTickVolume(volumeF);
		m_exc2->setTickVolume(volumeF);
		volumeLabel->setText(QString::number(_value) + "%");
		});

	QHBoxLayout* spinLayout = new QHBoxLayout;
	layout->addLayout(spinLayout, 1);
	
	m_multi = new RandomizerWheel(m_root);
	m_multi->addItems(getMultiplierOptions());
	m_multi->setTickVolume(initialVolumeF);

	spinLayout->addWidget(m_multi);
	connect(m_multi, &RandomizerWheel::spinFinished, this, &ExerciseWidget::slotMultiFinished);


	m_exc = new RandomizerWheel(m_root);
	m_exc->addItems(getExerciseOptions());
	m_exc->setTickVolume(initialVolumeF);
	//m_exc->setPlaySounds(true);
	spinLayout->addWidget(m_exc);
	connect(m_exc, &RandomizerWheel::spinFinished, this, &ExerciseWidget::slotExercise1Finished);


	m_exc2 = new RandomizerWheel(m_root);
	m_exc2->addItems(getExerciseOptions());
	m_exc2->setTickVolume(initialVolumeF);
	//m_exc2->setPlaySounds(true);
	spinLayout->addWidget(m_exc2);
	connect(m_exc2, &RandomizerWheel::spinFinished, this, &ExerciseWidget::slotExercise2Finished);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	layout->addLayout(buttonLayout);

	buttonLayout->addStretch(1);

	m_btn = new QPushButton("  Start  ", m_root);
	QFont btnFont = m_btn->font();
	btnFont.setPixelSize(64);
	m_btn->setFont(btnFont);

	buttonLayout->addWidget(m_btn);

	connect(m_btn, &QPushButton::clicked, this, &ExerciseWidget::slotStart);

	buttonLayout->addStretch(1);

	hideExercises();

	m_winnerEffect.setSource(QUrl(QStringLiteral("qrc:/winner.wav")));
	m_winnerEffect.setVolume(initialVolumeF);
}

void ExerciseWidget::slotMultiFinished(RandomizerWheelItem* _item)
{
	m_state.remove(State::WaitForMulti);
	EXERCISE_LOG("> Multiplier: " + _item->getText());

	const QVariant& data = _item->getData();
	if (!data.isValid() || !data.canConvert<int>())
	{
		m_btn->setEnabled(true);
		return;
	}
	
	OptionMode mode = static_cast<OptionMode>(data.toInt());
	showAndSpinExercises(mode == OptionMode::ShowExercise2 ? 2 : 1);
}

void ExerciseWidget::slotExercise1Finished(RandomizerWheelItem* _item)
{
	m_state.remove(State::WaitForExecise1);
	EXERCISE_LOG("> Exercise 1: " + _item->getText());
	checkForCompletion();
}

void ExerciseWidget::slotExercise2Finished(RandomizerWheelItem * _item)
{
	m_state.remove(State::WaitForExecise2);
	EXERCISE_LOG("> Exercise 2: " + _item->getText());
	checkForCompletion();
}

void ExerciseWidget::slotStart()
{
	if (m_state.any())
	{
		return;
	}

	m_state.set(State::IsRunning | State::WaitForMulti);

	m_btn->setEnabled(false);
	hideExercises();
	
	const int index1 = QRandomGenerator::global()->bounded(m_multi->getItemCount());

	m_multi->spinToIndex(index1);

	EXERCISE_LOG("Randomizing...");
}

ExerciseWidget::OptionList ExerciseWidget::getMultiplierOptions() const
{
	return OptionList({
		{"1.0x", QVariant(static_cast<int>(OptionMode::None))},
		{"1.5x Double", QVariant(static_cast<int>(OptionMode::ShowExercise2))},
		{"2.0x", QVariant(static_cast<int>(OptionMode::None))},
		{"2x Double", QVariant(static_cast<int>(OptionMode::ShowExercise2))},
		{"1.5x", QVariant(static_cast<int>(OptionMode::None))},
		{"1.0x Double", QVariant(static_cast<int>(OptionMode::ShowExercise2))}
		});
}

ExerciseWidget::OptionList ExerciseWidget::getExerciseOptions() const
{
	return OptionList({
		{"26 Sit-ups", QVariant(static_cast<int>(OptionMode::None))},
		{"10 weighted Sit-ups", QVariant(static_cast<int>(OptionMode::None))},
		{"26 Squads", QVariant(static_cast<int>(OptionMode::None))},
		{"10 weighted Squads", QVariant(static_cast<int>(OptionMode::None))},
		{"6 Pull-ups", QVariant(static_cast<int>(OptionMode::None))},
		{"6 half-full-half Pull-ups", QVariant(static_cast<int>(OptionMode::None))},
		{"6 +3s hold Pull-ups", QVariant(static_cast<int>(OptionMode::None))},
		{"10 explosive Push-ups", QVariant(static_cast<int>(OptionMode::None))},
		{"10 narrow Push-ups", QVariant(static_cast<int>(OptionMode::None))},
		{"10 wide Push-ups", QVariant(static_cast<int>(OptionMode::None))},
		{"10 incline Push-ups", QVariant(static_cast<int>(OptionMode::None))},
		{"1 min Plank", QVariant(static_cast<int>(OptionMode::None))},
		{"1 min Side-Plank each", QVariant(static_cast<int>(OptionMode::None))},
		});
}

void ExerciseWidget::hideExercises()
{
	m_exc->setVisible(false);
	m_exc2->setVisible(false);
}

void ExerciseWidget::showAndSpinExercises(int _n)
{
	if (_n >= 1)
	{
		m_exc->setVisible(true);
		const int index = QRandomGenerator::global()->bounded(m_exc->getItemCount());
		m_state.set(State::WaitForExecise1);
		QTimer::singleShot(500, [this, index]() { m_exc->spinToIndex(index); });
	}
	if (_n >= 2)
	{
		m_exc2->setVisible(true);
		m_state.set(State::WaitForExecise2);
		const int index = QRandomGenerator::global()->bounded(m_exc2->getItemCount());
		QTimer::singleShot(3500, [this, index]() { m_exc2->spinToIndex(index); });
	}
}

void ExerciseWidget::checkForCompletion()
{
	if (m_state == State::IsRunning)
	{
		m_state.remove(State::IsRunning);
		m_winnerEffect.play();
		m_btn->setEnabled(true);
	}
}
