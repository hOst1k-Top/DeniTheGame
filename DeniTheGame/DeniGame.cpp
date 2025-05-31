#include "include/DeniGame.h"

DeniGame::DeniGame(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DeniGame())
    , currentOverlay(nullptr)
    , messageQueue()
{
    ui->setupUi(this);
    scene = new InteractiveGraphicsScene(this);
    scene->setSceneRect(0, 0, 1000, 800);
    ui->MemoryField->setScene(scene);
    ui->MemoryField->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    showFullScreen();

    connect(&manager, &GameManager::gameStarted, this, &DeniGame::onGameStarted);
    connect(&manager, &GameManager::roundStarted, this, &DeniGame::onRoundStarted);
    connect(&manager, &GameManager::phaseChanged, this, &DeniGame::onPhaseChanged);
    connect(&manager, &GameManager::correctAnswer, this, &DeniGame::updateScore);
    connect(&manager, &GameManager::incorrectAnswer, this, &DeniGame::updateScore);
    connect(&manager, &GameManager::memoryCountChanged, this, &DeniGame::updateScore);
    connect(&manager, &GameManager::finalRoundStarted, this, &DeniGame::onFinalRoundStarted);
    connect(&manager, &GameManager::showMessageRequested, this, &DeniGame::onShowMessageRequested);

    connect(ui->pushButton, &QAbstractButton::clicked, [&]() {
        GamePhase phase = manager.getPhase();
        switch (phase)
        {
        case GamePhase::Setup:
            manager.startGame();
            break;
        case GamePhase::ActiveThinking:
        {
            std::vector<int> picked;
            for (int i = 0; i < ui->cardLayout->count(); ++i)
            {
                QLayoutItem* item = ui->cardLayout->itemAt(i);
                if (!item) continue;
                ClickableCard* card = qobject_cast<ClickableCard*>(item->widget());
                if (!card) continue;
                if (card->getState() == State::Selected) picked.push_back(card->getId());
            }
            manager.submitMemoryCards(picked);
            updateMemoryField();
            displaySubmittedMemoryCards(picked);
            break;
        }
        case GamePhase::Painting:
            manager.submitPaint();
            break;
        case GamePhase::Decision:
        {
            auto words = manager.getCurrentWords();
            QStringList items = words.keys();

            showSelectionDialog(tr("Word Selector"), tr("Select easter world:"), items,
                [this, words](const QString& selected) {
                    manager.makeDecision(words[selected]);
                });
            break;
        }
        case GamePhase::RoundEnd:
            manager.startNextRound();
            break;
        case GamePhase::FinalGuess:
        {
            QMap<QString, int> players;
            for (auto player : SettingsManager::getInstance()->getPlayers())
                players.insert(player.name, player.id);
            QStringList names = players.keys();

            showSelectionDialog(tr("Deny Picker"), tr("The Deny is:"), names,
                [this, players](const QString& picked) {
                    manager.finalGuess(players[picked]);
                });
            break;
        }
        default:
            break;
        }
    });

    connect(&manager, &GameManager::gameFinished, this, &DeniGame::onGameFinished);
}

DeniGame::~DeniGame()
{
    delete ui;
}

void DeniGame::onGameStarted()
{
    resetInterface();
}

void DeniGame::onRoundStarted(int round, const int& idea)
{
    ui->Title->setText(tr("Round %1").arg(round));
    updateIdeaCard(idea);
    updateMemoryField();
}

void DeniGame::onPhaseChanged(GamePhase phase)
{
    switch (phase)
    {
    case GamePhase::ActiveThinking:
        ui->pushButton->setText(tr("Submit"));
        ui->pushButton->setEnabled(true);
        if (scene) scene->setInteractionEnabled(false);
        break;
    case GamePhase::Painting:
        ui->pushButton->setText(tr("Submit Paint"));
        ui->pushButton->setEnabled(true);
        if (scene) scene->setInteractionEnabled(true);
        break;
    case GamePhase::OtherDiscuss:
        ui->pushButton->setText(tr("Waiting..."));
        ui->pushButton->setEnabled(false);
        if (scene) scene->setInteractionEnabled(false);
        break;
    case GamePhase::Decision:
        ui->pushButton->setText(tr("Make Decision"));
        ui->pushButton->setEnabled(true);
        if (scene) scene->setInteractionEnabled(false);
        break;
    case GamePhase::RoundEnd:
        ui->pushButton->setText(tr("Next Round"));
        ui->pushButton->setEnabled(true);
        if (scene) {
            scene->clear();
            scene->setInteractionEnabled(false);
        }
        break;
    case GamePhase::FinalGuess:
        ui->pushButton->setText(tr("The Deny is..."));
        ui->Title->setText(tr("Final Guessing"));
        if (scene) scene->setInteractionEnabled(false);
        break;
    default:
        break;
    }
}

void DeniGame::onFinalRoundStarted()
{
}

void DeniGame::onGameFinished(bool altersWin)
{
    QString message = altersWin ? tr("Alters Win!") : tr("Dany Wins!");
    message += tr("\n\nContinue?");

    showSelectionDialog(tr("Game Over"), message, { tr("New Game"), tr("Exit") },
        [this](const QString& choice) {
            if (choice == tr("Exit")) {
                QApplication::quit();
            }
            else {
                GameInitScreen* settings = new GameInitScreen();
                close();
                settings->show();
                deleteLater();
            }
    });
}

void DeniGame::onShowMessageRequested(const QString& title, const QString& text)
{
    MessageInfo info;
    info.title = title;
    info.text = text;
    info.isInfo = true;

    messageQueue.enqueue(info);

    if (!currentOverlay) {
        processNextMessage();
    }
}

void DeniGame::showSelectionDialog(const QString& title, const QString& message,
    const QStringList& items,
    std::function<void(const QString&)> onSelected)
{
    MessageInfo info;
    info.title = title;
    info.text = message;
    info.isInfo = false;
    info.items = items;
    info.selectionCallback = std::move(onSelected);

    messageQueue.enqueue(info);

    if (!currentOverlay) {
        processNextMessage();
    }
}

void DeniGame::processNextMessage()
{
    if (messageQueue.isEmpty()) {
        return;
    }

    MessageInfo info = messageQueue.dequeue();
    currentOverlay = new GameOverlayDialog(this);

    if (info.isInfo) {
        currentOverlay->showInfo(info.text, [this]() {
            currentOverlay = nullptr;
            QTimer::singleShot(100, this, &DeniGame::processNextMessage);
            });
    }
    else {
        currentOverlay->showSelection(info.text, info.items,
            [this, callback = info.selectionCallback](const QString& selected) {
                currentOverlay = nullptr;
                if (callback) callback(selected);
                QTimer::singleShot(100, this, &DeniGame::processNextMessage);
            },
            [this]() {
                currentOverlay = nullptr;
                QTimer::singleShot(100, this, &DeniGame::processNextMessage);
            }
        );
    }
}

void DeniGame::resetInterface()
{
    ui->deniScore->display(0);
    ui->alterScore->display(0);
    ui->Title->setText(tr("Deni: Voices In My Head"));
    ui->ideaCard->clear();
}

void DeniGame::updateIdeaCard(const int& id)
{
    ui->ideaCard->setPixmap(GameManager::getIdeaCardImage(id).scaled(256, 387));
}

void DeniGame::displaySubmittedMemoryCards(const std::vector<int>& picked)
{
    if (!scene) return;
    scene->clear();
    const int spacing = 30;
    const int cardWidth = 200;
    const int cardHeight = 300;
    int x = 50, y = 50;
    for (int id : picked)
    {
        auto* card = new MemoryCardItem(id);
        card->setPos(x, y);
        scene->addItem(card);
        x += cardWidth + spacing;
        if (x + cardWidth > scene->width()) {
            x = 50;
            y += cardHeight + spacing;
        }
    }
}

void DeniGame::updateScore()
{
    ui->alterScore->display(manager.getCorrectAnswers());
    ui->deniScore->display(manager.getIncorrectAnswers());
    ui->remain->display(manager.getRemainCards());
}

void DeniGame::updateMemoryField()
{
    const auto& cards = manager.getActivePlayer().currentHend;
    auto labels = { ui->label_3, ui->label_4, ui->label_5, ui->label_6, ui->label_9, ui->label_7, ui->label_8 };
    size_t index = 0;
    for (ClickableCard* label : labels)
    {
        if (index < cards.size())
        {
            label->setPixmap(manager.getMemoryCardImage(cards[index]).scaled(240, 387, Qt::KeepAspectRatio));
            label->setId(cards[index]);
            label->setInteraction(true);
        }
        else
        {
            label->clear();
            label->setInteraction(false);
        }
        label->resetState();
        ++index;
    }
}