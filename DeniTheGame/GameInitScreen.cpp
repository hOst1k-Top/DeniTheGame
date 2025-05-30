#include "include/GameInitScreen.h"

GameInitScreen::GameInitScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameInitScreenClass())
{
    ui->setupUi(this);
    setFixedHeight(height());
    ui->memoryCards->addItem(tr("30 cards"), 30);
    ui->memoryCards->addItem(tr("35 cards"), 35);
    ui->memoryCards->addItem(tr("40 cards"), 40);
    QObject::connect(ui->pCount, &QSpinBox::valueChanged, this, &GameInitScreen::handlePlayerCountChange);
    QObject::connect(ui->startGame, &QAbstractButton::clicked, [this]() {
        auto players = getPlayers();
        if (players.isEmpty())
        {
            QMessageBox::warning(this, tr("Error!"), tr("Player Names is not selected"));
            return;
        }
        SettingsManager::getInstance()->setMemoryToPick(ui->memoryCards->currentData().toInt());
        SettingsManager::getInstance()->setPlayers(getPlayers());

        DeniGame* game = new DeniGame();
        game->show();
        close();
    });
}

GameInitScreen::~GameInitScreen()
{
    delete ui;
}

QMap<int, Player> GameInitScreen::getPlayers()
{
    QMap<int, Player> map;
    int pCount = ui->pCount->value();
    int totalRows = ui->formLayout_2->rowCount();

    for (int row = 0; row < totalRows; ++row)
    {
        QLayoutItem* fieldItem = ui->formLayout_2->itemAt(row, QFormLayout::FieldRole);
        if (!fieldItem) continue;

        QLineEdit* edit = qobject_cast<QLineEdit*>(fieldItem->widget());
        if (!edit) continue;

        if (edit->isEnabled() && !edit->text().isEmpty()) map.insert(row, Player(edit->text(), row));
    }
    return map;
}

void GameInitScreen::handlePlayerCountChange()
{
    int pCount = ui->pCount->value();
    int totalRows = ui->formLayout_2->rowCount();

    for (int row = 0; row < totalRows; ++row)
    {
        QLayoutItem* fieldItem = ui->formLayout_2->itemAt(row, QFormLayout::FieldRole);
        if (!fieldItem) continue;

        QLineEdit* edit = qobject_cast<QLineEdit*>(fieldItem->widget());
        if (!edit) continue;

        if (row < pCount)
        {
            if (!edit->isEnabled()) edit->setEnabled(true);
        }
        else
        {
            edit->setEnabled(false);
            edit->clear();
        }
    }
}
