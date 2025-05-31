#pragma once

#include <QObject>
#include <QMessageBox>
#include <random>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include "SettingsManager.h"
#include "CardDealler.h"
#include "dbprovider.h"

enum class GamePhase {
    Setup,
    RoundStart,
    ActiveThinking,
    Painting,
    OtherDiscuss,
    Decision,
    Reveal,
    RoundEnd,
    FinalGuess,
    GameOver
};

class GameManager : public QObject
{
    Q_OBJECT
    Q_ENUM(GamePhase)

public:
    GameManager(QObject* parent = nullptr);

    void startGame();
    void startNextRound();
    void submitMemoryCards(std::vector<int>& selected);
    void submitPaint();
    void makeDecision(int guessedIndex);
    void submitPlayerVote(int votingPlayerId, int suspectedDanyId);

    GamePhase getPhase() const;
    int getCorrectAnswers() const;
    int getIncorrectAnswers() const;
    int getRemainCards();
    const Player& getActivePlayer() const;
    const Player& getDecidingPlayer() const;
    std::vector<int> getCurrentIdeaWordOptions() const;
    QString getCurrentIdeaText() const;
    const QMap<QString, int>& getCurrentWords() const { return currentWords; }

    QStringList getPlayerNamesForVoting(int excludePlayerId = -1) const;
    QMap<QString, int> getPlayerMapForVoting(int excludePlayerId = -1) const;

    bool isGameOver() const;

    static QPixmap getMemoryCardImage(int id);
    static QPixmap getIdeaCardImage(int id);

signals:
    void gameStarted();
    void roundStarted(int roundNumber, int idea);
    void phaseChanged(GamePhase newPhase);
    void correctAnswer();
    void incorrectAnswer();
    void finalRoundStarted();
    void gameFinished(bool altersWin);
    void memoryCountChanged();
    void showMessageRequested(const QString& title, const QString& text);
    void requestPlayerVote(int currentPlayerId);

private:
    void rotateRoles();
    bool checkGameOver();
    void cleanupRound();
    void assignRoles();

    void startFinalVoting();
    void startNextPlayerVote();
    void processFinalVotes();
    void finalizeSingleCandidate(int candidateId);
    void handleSplitVoting(const QMap<int, int>& votesCounts);
    void startFinalCandidateVoting(const QSet<int>& candidates, const QMap<int, int>& votesCounts);

private:
    GamePhase phase;
    int activePlayerId;
    int decidingPlayerId;
    int roundNumber;

    int currentIdeaId;
    QMap<QString, int> currentWords;
    QString currentIdeaText;
    int ideaWordIndex;

    int correctAnswers;
    int incorrectAnswers;

    QMap<int, int> finalVotes;
    QList<int> votingPlayers;
    int currentVotingPlayerIndex;

    std::map<int, Player> players;

    CardDealler dealer;
    std::mt19937 rng;
};
