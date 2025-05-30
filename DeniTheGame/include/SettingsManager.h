#pragma once

#include <QString>
#include <QMap>
#include "Player.h"


class SettingsManager
{
public:
	~SettingsManager();
	SettingsManager(SettingsManager& other) = delete;
	void operator=(SettingsManager other) = delete;
	static SettingsManager* getInstance();
	
public:
	int getMemoryToPick() const { return memoryPick; };
	void setMemoryToPick(int pick) { memoryPick = pick; };
	int getDiscussTime() const { return discussionTime; };
	void setDiscussTime(int secs) { discussionTime = secs; }
	QMap<int, Player> getPlayers() { return players; };
	void setPlayers(QMap<int, Player> pl) { players = pl; };


private:
	SettingsManager();
	int memoryPick, discussionTime;
	QMap<int, Player> players;
};
