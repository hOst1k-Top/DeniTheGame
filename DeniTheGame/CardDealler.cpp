#include "include/CardDealler.h"

CardDealler::CardDealler()
{
}

CardDealler::~CardDealler()
{
}

void CardDealler::InitCards()
{
    std::vector<int> values(MemoryCount);
    std::iota(values.begin(), values.end(), 1);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::shuffle(values.begin(), values.end(), mt);
    memory = std::vector<int>(values.begin(), values.begin() + SettingsManager::getInstance()->getMemoryToPick());

    ideas = std::vector<int>(IdeaCount);
    std::iota(ideas.begin(), ideas.end(), 1);
    std::shuffle(ideas.begin(), ideas.end(), mt);
}

int CardDealler::takeIdea()
{
    int id = ideas.back();
    ideas.pop_back();
    return id;
}

std::vector<int> CardDealler::takeMemory()
{
    if (memory.size() < 7)
    {
        return std::vector<int>();
    }
    std::vector<int> memories;
    for (int i = 0; i < 7; ++i)
    {
        memories.push_back(memory.back());
        memory.pop_back();
    }
    return memories;
}

void CardDealler::returnMemory(std::vector<int> toReturn)
{
    memory.insert(memory.end(), toReturn.begin(), toReturn.end());
}
