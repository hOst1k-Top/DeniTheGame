#pragma once

#include <QWidget>
#include <QMap>
#include <QInputDialog>
#include "../ui/ui_DeniGame.h"
#include "GameManager.h"
#include "ClickableCard.h"
#include "GameInitScreen.h"
#include "MemoryCardItem.h"
#include "InteractiveGraphicsScene.h"

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
    void onFinalRoundStarted();
    void onGameFinished(bool altersWin);
    void updateScore();

private:
	Ui::DeniGame *ui;
	GameManager manager;
    InteractiveGraphicsScene* scene;

    void resetInterface();
    void updateIdeaCard(const int& id);
    void displaySubmittedMemoryCards(const std::vector<int>& picked);
    void updateMemoryField();
};
