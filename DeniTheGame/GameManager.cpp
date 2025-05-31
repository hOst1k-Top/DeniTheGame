#include "include/GameManager.h"

GameManager::GameManager(QObject* parent) :
    QObject(parent),
    phase(GamePhase::Setup),
    activePlayerId(1),
    decidingPlayerId(2),
    roundNumber(0),
    correctAnswers(0),
    incorrectAnswers(0),
    rng(std::random_device{}()),
    currentVotingPlayerIndex(0)
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

    emit showMessageRequested(tr("Game"), tr("The game has started."));

    for (auto& [_, player] : players) {
        emit showMessageRequested(tr("Game Start"), tr("Player %1 see the screen. Other close eyes.").arg(player.name));
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
        emit showMessageRequested(tr("Role"), roleText);
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
        emit showMessageRequested(tr("Database Error"), ideaGet.lastError().text());
        return;
    }

    if (ideaGet.first())
    {
        currentIdeaText = ideaGet.value(1).toString();
        ideaWordIndex = ideaGet.value(0).toInt();
    }

    QSqlQuery allWordsGet(DBProvider::getInstance()->getDB());
    allWordsGet.prepare("SELECT word,wordindex FROM Ideas WHERE ideaid = :id");
    allWordsGet.bindValue(":id", currentIdeaId);
    if (!allWordsGet.exec())
    {
        emit showMessageRequested(tr("Database Error"), allWordsGet.lastError().text());
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
    emit showMessageRequested(tr("Round %1: Players Role").arg(roundNumber), tr("Active Player: %1\nDeciding Player: %2").arg(players[activePlayerId].name).arg(players[decidingPlayerId].name));
    emit showMessageRequested(tr("Active Move"), tr("Time for memory, active player. Other close eys."));
    emit showMessageRequested(tr("Picked Word"), QString("%1: %2").arg(ideaWordIndex).arg(currentIdeaText));
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
    std::ranges::set_difference(hand, selected, std::back_inserter(toReturn));
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
    emit showMessageRequested(tr("Answer"), correct ? tr("Correct answer!") : tr("Wrong answer! Correct answer is %1").arg(currentIdeaText));

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

    if (!checkGameOver())
    {
        phase = GamePhase::RoundEnd;
        emit phaseChanged(phase);
    }
}

bool GameManager::checkGameOver()
{
    if (correctAnswers >= 6 || incorrectAnswers >= 3) {
        startFinalVoting();
        return true;
    }
    return false;
}

void GameManager::startFinalVoting()
{
    phase = GamePhase::FinalGuess;
    finalVotes.clear();
    currentVotingPlayerIndex = 0;

    votingPlayers.clear();
    for (auto [id, player] : players) {
        votingPlayers.append(id);
    }

    emit showMessageRequested(tr("Final Round"), tr("Final voting begins. Each player will vote individually."));
    emit phaseChanged(phase);
    startNextPlayerVote();
}

void GameManager::startNextPlayerVote()
{
    if (currentVotingPlayerIndex >= votingPlayers.size()) {
        processFinalVotes();
        return;
    }

    int currentPlayerId = votingPlayers[currentVotingPlayerIndex];
    QString currentPlayerName = players[currentPlayerId].name;

    emit showMessageRequested(tr("Voting Turn"), tr("Player %1, it's your turn to vote. Others close eyes.").arg(currentPlayerName));

    emit requestPlayerVote(currentPlayerId);
}

void GameManager::submitPlayerVote(int votingPlayerId, int suspectedDanyId)
{
    finalVotes[votingPlayerId] = suspectedDanyId;
    currentVotingPlayerIndex++;

    QString voterName = players[votingPlayerId].name;
    QString suspectedName = players[suspectedDanyId].name;
    emit showMessageRequested(tr("Vote Recorded"), tr("%1 voted for %2").arg(voterName).arg(suspectedName));

    startNextPlayerVote();
}

void GameManager::processFinalVotes()
{
    QMap<int, int> votesCounts;
    for (auto vote : finalVotes.values()) {
        votesCounts[vote]++;
    }

    int maxVotes = 0;
    QList<int> topCandidates;

    for (auto it = votesCounts.begin(); it != votesCounts.end(); ++it) {
        if (it.value() > maxVotes) {
            maxVotes = it.value();
            topCandidates.clear();
            topCandidates.append(it.key());
        }
        else if (it.value() == maxVotes) {
            topCandidates.append(it.key());
        }
    }

    if (topCandidates.size() == 1 && maxVotes == players.size()) {
        finalizeSingleCandidate(topCandidates.first());
    }
    else {
        handleSplitVoting(votesCounts);
    }
}

void GameManager::finalizeSingleCandidate(int candidateId)
{
    bool altersWin = (players[candidateId].role == Role::Dany);

    QString candidateName = players[candidateId].name;
    QString result = altersWin ? tr("Correct! %1 is Dany. Alters win!").arg(candidateName)
        : tr("Wrong! %1 is not Dany. Dany wins!").arg(candidateName);

    emit showMessageRequested(tr("Final Result"), result);
    emit gameFinished(altersWin);
    phase = GamePhase::GameOver;
    emit phaseChanged(phase);
}

void GameManager::handleSplitVoting(const QMap<int, int>& votesCounts)
{
    emit showMessageRequested(tr("Split Voting"), tr("Opinions are divided. Non-candidates will reveal their identities."));

    QSet<int> candidates;
    for (auto candidateId : votesCounts.keys()) {
        candidates.insert(candidateId);
    }

    QSet<int> nonCandidates;
    for (auto [voterId, _] : finalVotes.toStdMap()) {
        if (!candidates.contains(voterId)) {
            nonCandidates.insert(voterId);
        }
    }

    bool danyAmongNonCandidates = false;
    QString revealMessage = tr("Non-candidates reveal:\n");

    for (int playerId : nonCandidates) {
        QString playerName = players[playerId].name;
        QString roleText = (players[playerId].role == Role::Dany) ? tr("Dany") : tr("Alter");
        revealMessage += tr("%1: %2\n").arg(playerName).arg(roleText);

        if (players[playerId].role == Role::Dany) {
            danyAmongNonCandidates = true;
        }
    }

    emit showMessageRequested(tr("Identity Reveal"), revealMessage);

    if (danyAmongNonCandidates) {
        emit showMessageRequested(tr("Final Result"), tr("Dany was among the voters! Dany wins!"));
        emit gameFinished(false);
    }
    else {
        startFinalCandidateVoting(candidates, votesCounts);
    }

    phase = GamePhase::GameOver;
    emit phaseChanged(phase);
}

void GameManager::startFinalCandidateVoting(const QSet<int>& candidates, const QMap<int, int>& votesCounts)
{
    emit showMessageRequested(tr("Final Candidate Voting"), tr("All voters are Alters. Final voting between candidates begins."));

    int maxVotes = 0;
    int topCandidate = -1;

    for (auto it = votesCounts.begin(); it != votesCounts.end(); ++it) {
        if (candidates.contains(it.key()) && it.value() > maxVotes) {
            maxVotes = it.value();
            topCandidate = it.key();
        }
    }

    if (topCandidate != -1) {
        QString candidateName = players[topCandidate].name;
        bool isDany = (players[topCandidate].role == Role::Dany);

        QString result;
        if (isDany) {
            result = tr("The most voted candidate %1 is Dany! Alters win!").arg(candidateName);
            emit gameFinished(true);
        }
        else {
            result = tr("The most voted candidate %1 is an Alter! Dany wins!").arg(candidateName);
            emit gameFinished(false);
        }

        emit showMessageRequested(tr("Final Result"), result);
    }
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

QStringList GameManager::getPlayerNamesForVoting(int excludePlayerId) const
{
    QStringList names;
    for (auto [id, player] : players) {
        if (id != excludePlayerId) {
            names.append(player.name);
        }
    }
    return names;
}

QMap<QString, int> GameManager::getPlayerMapForVoting(int excludePlayerId) const
{
    QMap<QString, int> playerMap;
    for (auto [id, player] : players) {
        if (id != excludePlayerId) {
            playerMap[player.name] = id;
        }
    }
    return playerMap;
}

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