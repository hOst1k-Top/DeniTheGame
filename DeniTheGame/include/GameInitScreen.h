#pragma once

#include <QtWidgets/QWidget>
#include "SettingsManager.h"
#include "Player.h"
#include "DeniGame.h"
#include "../ui/ui_GameInitScreen.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GameInitScreenClass; };
QT_END_NAMESPACE

class GameInitScreen : public QWidget
{
    Q_OBJECT

public:
    GameInitScreen(QWidget *parent = nullptr);
    ~GameInitScreen();
    QMap<int, Player> getPlayers();

public:
    void handlePlayerCountChange();

private:
    Ui::GameInitScreenClass *ui;
};
