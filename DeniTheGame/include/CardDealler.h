#pragma once

#include <QPixmap>
#include <QFile>
#include <vector>
#include <random>
#include <QVector>
#include <algorithm>
#include "SettingsManager.h"

class CardDealler
{
public:
	CardDealler();
	~CardDealler();

	void InitCards();
	int takeIdea();
	std::vector<int> takeMemory();
	void returnMemory(std::vector<int> toReturn);

private:
	const int MemoryCount = 60, IdeaCount = 37;
	std::vector<int> memory;
	std::vector<int> ideas;

};
