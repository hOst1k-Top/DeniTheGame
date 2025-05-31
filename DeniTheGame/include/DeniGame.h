#pragma once

#include <QWidget>
#include <QMap>
#include <QInputDialog>
#include <QQueue>
#include "../ui/ui_DeniGame.h"
#include "GameManager.h"
#include "ClickableCard.h"
#include "GameInitScreen.h"
#include "MemoryCardItem.h"
#include "InteractiveGraphicsScene.h"
#include "GameOverlayDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DeniGame; };
QT_END_NAMESPACE

struct MessageInfo {
    QString title;
    QString text;
    bool isInfo;
    QStringList items;
    std::function<void(const QString&)> selectionCallback;
};

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
    void onRequestPlayerVote(int currentPlayerId);
    void updateScore();
    void onShowMessageRequested(const QString& title, const QString& text);

private:
	Ui::DeniGame *ui;
	GameManager manager;
    InteractiveGraphicsScene* scene;
    GameOverlayDialog* currentOverlay;
    QQueue<MessageInfo> messageQueue;

    void resetInterface();
    void updateIdeaCard(const int& id);
    void displaySubmittedMemoryCards(const std::vector<int>& picked);
    void updateMemoryField();
    void showSelectionDialog(const QString& title, const QString& message,
        const QStringList& items,
        std::function<void(const QString&)> onSelected);
    void processNextMessage();
};
