#pragma once

#include <QWidget>
#include "../ui/ui_DeniGame.h"
#include "GameManager.h"
#include "ClickableCard.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DeniGame; };
QT_END_NAMESPACE

class DeniGame : public QWidget
{
	Q_OBJECT

public:
	DeniGame(QWidget *parent = nullptr);
	~DeniGame();

private slots:
    void onGameStarted();
    void onRoundStarted(int round, const int& idea);
    void onPhaseChanged(GamePhase phase);
    void onCorrectAnswer();
    void onIncorrectAnswer();
    void onFinalRoundStarted();
    void onGameFinished(bool altersWin);

private:
	Ui::DeniGame *ui;
	GameManager manager;

    void resetInterface();
    void updateIdeaCard(const int& id);
    void updateScore();
    void updateMemoryField();
    void switchToFinalPhase();
};
