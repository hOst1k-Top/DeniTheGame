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

    rotateRoles();
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

    players[activePlayerId].currentHend = dealer.takeMemory();

    ++roundNumber;
    phase = GamePhase::ActiveThinking;

    emit roundStarted(roundNumber, currentIdeaId);
    emit phaseChanged(phase);
}

void GameManager::submitMemoryCards(const std::vector<int>& selected)
{
    std::vector<int> toReturn;

    std::ranges::set_difference(players[activePlayerId].currentHend,
        selected,
        std::back_inserter(toReturn));

    dealer.returnMemory(toReturn);
    players[activePlayerId].currentHend = selected;

    phase = GamePhase::OtherDiscuss;

    emit phaseChanged(phase);
}

void GameManager::makeDecision(int guessedIndex)
{
    bool correct = (guessedIndex == ideaWordIndex);

    QMessageBox::information(nullptr, tr("Answer"),
        correct ? tr("Correct answer!") : tr("Wrong answer!"));

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

    checkGameOver();
    phase = GamePhase::RoundEnd;
    emit phaseChanged(phase);
}

void GameManager::checkGameOver()
{
    if (correctAnswers >= 6 || incorrectAnswers >= 3) {
        phase = GamePhase::FinalGuess;
        QMessageBox::information(nullptr, tr("Final Round"), tr("Final guessing begins."));
        emit finalRoundStarted();
        emit phaseChanged(phase);
    }
}

void GameManager::finalGuess(int playerId)
{
    if (players[playerId].role == Role::Dany)
        QMessageBox::information(nullptr, tr("Result"), tr("Dany is found. Alters win!"));
    else
        QMessageBox::information(nullptr, tr("Result"), tr("Wrong guess. Dany wins."));

    phase = GamePhase::GameOver;
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