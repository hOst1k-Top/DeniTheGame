#include "include/GameManager.h"

GameManager::GameManager(QObject* parent):
    QObject(parent),
    phase(GamePhase::Setup),
    activePlayerId(1),
    decidingPlayerId(2),
    roundNumber(0),
    correctAnswers(0),
    incorrectAnswers(0),
    rng(std::random_device{}())
{
	dealer.InitCards();
    emit memoryCountChanged();
	auto temp = SettingsManager::getInstance()->getPlayers();
	for (auto key : temp.keys())
	{
		players.insert(std::make_pair(key, temp[key]));
	}
}

void GameManager::startGame()
{
    assignRoles();
    for (auto& [_, player] : players) {
        QMessageBox::information(nullptr, tr("Game Start"),
            tr("Player %1 see the screen. Other close eyes.").arg(player.name));

        QString roleText;
        switch (player.role) {
        case Role::Dany: 
            roleText = tr("You are Dany"); 
            break;
        case Role::Alter: 
            roleText = tr("You are Alter personality"); 
            break;
        default: 
            roleText = tr("Unknown role"); 
            break;
        }
        QMessageBox::information(nullptr, tr("Role"), roleText);
    }

    emit gameStarted();
    startNextRound();
}

void GameManager::rotateRoles()
{

    QList<int> ids;
    for (auto [id, _] : players) ids.push_back(id);
    std::sort(ids.begin(), ids.end());

    int index = ids.indexOf(activePlayerId);
    activePlayerId = ids[(index + 1) % ids.size()];
    decidingPlayerId = ids[(index + 2) % ids.size()];
}

void GameManager::startNextRound()
{
    if (phase == GamePhase::GameOver) return;
    currentWords.clear();

    currentIdeaId = dealer.takeIdea();
    QSqlQuery ideaGet(DBProvider::getInstance()->getDB());
    ideaGet.prepare("SELECT wordindex, word FROM Ideas WHERE ideaid = :id ORDER BY random()");
    ideaGet.bindValue(":id", currentIdeaId);
    if (!ideaGet.exec())
    {
        QMessageBox::warning(nullptr, tr("Database Error"), ideaGet.lastError().text());
        return;
    }
    if(ideaGet.first())
    {
        currentIdeaText = ideaGet.value(1).toString();
        ideaWordIndex = ideaGet.value(0).toInt();
    }

    QSqlQuery allWordsGet(DBProvider::getInstance()->getDB());
    allWordsGet.prepare("SELECT word,wordindex FROM Ideas WHERE ideaid = :id");
    allWordsGet.bindValue(":id", currentIdeaId);
    if (!allWordsGet.exec())
    {
        QMessageBox::warning(nullptr, tr("Database Error"), allWordsGet.lastError().text());
        return;
    }
    while (allWordsGet.next())
    {
        currentWords.insert(allWordsGet.value(0).toString(), allWordsGet.value(1).toInt());
    }

    rotateRoles();
    players[activePlayerId].currentHend = dealer.takeMemory();
    emit memoryCountChanged();

    ++roundNumber;
    QMessageBox::information(nullptr, tr("Round %1: Players Role").arg(roundNumber), tr("Active Player: %1\nDeciding Player: %2").arg(players[activePlayerId].name).arg(players[decidingPlayerId].name));
    QMessageBox::information(nullptr, tr("Active Move"), tr("Time for memory, active player. Other close eys."));
    QMessageBox::information(nullptr, tr("Picked Word"), QString("%1: %2").arg(ideaWordIndex).arg(currentIdeaText));
    phase = GamePhase::ActiveThinking;

    emit roundStarted(roundNumber, currentIdeaId);
    emit phaseChanged(phase);
}

void GameManager::submitMemoryCards(std::vector<int>& selected)
{
    std::vector<int> toReturn;
    auto& hand = players[activePlayerId].currentHend;
    std::sort(selected.begin(), selected.end());
    std::sort(hand.begin(), hand.end());

    std::ranges::set_difference(hand,
        selected,
        std::back_inserter(toReturn));

    dealer.returnMemory(toReturn);
    emit memoryCountChanged();
    players[activePlayerId].currentHend.clear();

    phase = GamePhase::Painting;
    emit phaseChanged(phase);
}

void GameManager::submitPaint()
{
    phase = GamePhase::OtherDiscuss;
    QTimer::singleShot(SettingsManager::getInstance()->getDiscussTime() * 1000, [&]() {
        phase = GamePhase::Decision;
        emit phaseChanged(phase);
        });

    emit phaseChanged(phase);
}

void GameManager::makeDecision(int guessedIndex)
{
    bool correct = (guessedIndex == ideaWordIndex);
    
    phase = GamePhase::Reveal;
    emit phaseChanged(phase);

    QMessageBox::information(nullptr, tr("Answer"),
        correct ? tr("Correct answer!") : tr("Wrong answer! Correct answer is %1").arg(currentIdeaText));

    if (correct)
    {
        ++correctAnswers;
        emit correctAnswer();
    }
    else
    {
        ++incorrectAnswers;
        emit incorrectAnswer();
    }

    if(!checkGameOver())
    {
        phase = GamePhase::RoundEnd;
        emit phaseChanged(phase);
    }
}

bool GameManager::checkGameOver()
{
    if (correctAnswers >= 6 || incorrectAnswers >= 3) {
        phase = GamePhase::FinalGuess;
        QMessageBox::information(nullptr, tr("Final Round"), tr("Final guessing begins."));
        emit phaseChanged(phase);
        return true;
    }
    return false;
}

void GameManager::finalGuess(int playerId)
{
    bool altersWin = false;
    if (players[playerId].role == Role::Dany)
    {
        QMessageBox::information(nullptr, tr("Result"), tr("Dany is found. Alters win!"));
        altersWin = true;
    }
    else QMessageBox::information(nullptr, tr("Result"), tr("Wrong guess. Dany wins."));

    emit gameFinished(altersWin);
    phase = GamePhase::GameOver;
    emit phaseChanged(phase);
}

void GameManager::cleanupRound()
{
    players[activePlayerId].currentHend.clear();

    rotateRoles();
    startNextRound();
}

void GameManager::assignRoles()
{
    std::vector<int> ids;
    for (auto [id, _] : players) ids.push_back(id);

    std::shuffle(ids.begin(), ids.end(), rng);

    int alterCount = ids.size() - 1;
    for (int i = 0; i < alterCount; ++i)
        players[ids[i]].role = Role::Alter;

    players[ids.back()].role = Role::Dany;
}

GamePhase GameManager::getPhase() const { return phase; }
int GameManager::getCorrectAnswers() const { return correctAnswers; }
int GameManager::getIncorrectAnswers() const { return incorrectAnswers; }
int GameManager::getRemainCards() { return dealer.getMemory(); }
const Player& GameManager::getActivePlayer() const { return players.at(activePlayerId); }
const Player& GameManager::getDecidingPlayer() const { return players.at(decidingPlayerId); }
std::vector<int> GameManager::getCurrentIdeaWordOptions() const { return { 1, 2, 3, 4, 5 }; }
QString GameManager::getCurrentIdeaText() const { return currentIdeaText; }
bool GameManager::isGameOver() const { return phase == GamePhase::GameOver; }

QPixmap GameManager::getMemoryCardImage(int id)
{
    QString path = QString(":/Memory/MemoryCards/%1.jpg").arg(id);
    return QFile::exists(path) ? QPixmap(path) : QPixmap();
}

QPixmap GameManager::getIdeaCardImage(int id)
{
    QString path = QString(":/Idea/IdeaCards/%1.jpg").arg(id);
    return QFile::exists(path) ? QPixmap(path) : QPixmap();
}