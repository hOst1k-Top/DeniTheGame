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
	int getMemoryToPick() { return memoryPick; };
	void setMemoryToPick(int pick) { memoryPick = pick; };
	QMap<int, Player> getPlayers() { return players; };
	void setPlayers(QMap<int, Player> pl) { players = pl; };

private:
	SettingsManager();
	int memoryPick;
	QMap<int, Player> players;
};
