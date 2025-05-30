#include "include/DeniGame.h"

DeniGame::DeniGame(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::DeniGame())
{
	ui->setupUi(this);
	showFullScreen();
    connect(&manager, &GameManager::gameStarted, this, &DeniGame::onGameStarted);
    connect(&manager, &GameManager::roundStarted, this, &DeniGame::onRoundStarted);
    connect(&manager, &GameManager::phaseChanged, this, &DeniGame::onPhaseChanged);
    connect(&manager, &GameManager::correctAnswer, this, &DeniGame::onCorrectAnswer);
    connect(&manager, &GameManager::incorrectAnswer, this, &DeniGame::onIncorrectAnswer);
    connect(&manager, &GameManager::finalRoundStarted, this, &DeniGame::onFinalRoundStarted);

    // TODO: При реализации gameFinished — подключить сигнал
    // connect(&manager, &GameManager::gameFinished, this, &DeniGame::onGameFinished);

    // Запуск игры
    manager.startGame();
}

DeniGame::~DeniGame()
{
	delete ui;
}

void DeniGame::onGameStarted()
{
    resetInterface();
    QMessageBox::information(this, tr("Game"), tr("The game has started."));
}

void DeniGame::onRoundStarted(int round, const int& idea)
{
    ui->Title->setText(tr("Round %1").arg(round));
    updateIdeaCard(idea);
    updateMemoryField();
}

void DeniGame::onPhaseChanged(GamePhase phase)
{
    // Пример адаптивного интерфейса по фазам
    switch (phase)
    {
    case GamePhase::ActiveThinking:
        ui->pushButton->setText("Submit");
        ui->pushButton->setEnabled(true);
        break;
    case GamePhase::OtherDiscuss:
        ui->pushButton->setText("Waiting...");
        ui->pushButton->setEnabled(false);
        break;
    case GamePhase::RoundEnd:
        ui->pushButton->setText("Next Round");
        ui->pushButton->setEnabled(true);
        break;
    case GamePhase::FinalGuess:
        switchToFinalPhase();
        break;
    default:
        break;
    }
}

void DeniGame::onCorrectAnswer()
{
    QMessageBox::information(this, tr("Answer"), tr("Correct!"));
    updateScore();
}

void DeniGame::onIncorrectAnswer()
{
    QMessageBox::warning(this, tr("Answer"), tr("Incorrect!"));
    updateScore();
}

void DeniGame::onFinalRoundStarted()
{
    QMessageBox::information(this, tr("Final Round"), tr("Final guessing begins."));
    // Логика переключения на финальный режим игры
}

void DeniGame::onGameFinished(bool altersWin)
{
    QString resultText = altersWin
        ? tr("Alters win!")
        : tr("Dany wins!");

    QMessageBox::information(this, tr("Game Over"), resultText);
    // TODO: Дать пользователю возможность перезапуска
}

void DeniGame::resetInterface()
{
    ui->deniScore->display(0);
    ui->alterScore->display(0);
    ui->Title->setText(tr("Deni: Voices In My Head"));
    ui->ideaCard->clear();
    // Очистка MemoryField, карточек и прочего
}

void DeniGame::updateIdeaCard(const int& id)
{
    ui->ideaCard->setPixmap(GameManager::getIdeaCardImage(id).scaled(256,387));
}

void DeniGame::updateScore()
{
    ui->deniScore->display(manager.getCorrectAnswers());
    ui->alterScore->display(manager.getIncorrectAnswers());
}

void DeniGame::updateMemoryField()
{
    const auto& cards = manager.getActivePlayer().currentHend;
    auto labels = { ui->label_3, ui->label_4, ui->label_5, ui->label_6, ui->label_9, ui->label_7, ui->label_8 };
    int index = 0;
    for (ClickableCard* label : labels)
    {
        if (index < static_cast<int>(cards.size()))
            label->setPixmap(manager.getMemoryCardImage(cards[index]).scaled(256,387, Qt::KeepAspectRatio));
        else
            label->clear();
        ++index;
    }
}

void DeniGame::switchToFinalPhase()
{
    // Финальный этап: скрытие ненужного, выделение главного
    ui->Title->setText(tr("Final Guessing"));
    // Можно дополнительно скрыть элементы MemoryField и т.п.
}
