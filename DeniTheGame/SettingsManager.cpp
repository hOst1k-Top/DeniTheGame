#include "include/SettingsManager.h"

SettingsManager::SettingsManager()
{
}

SettingsManager::~SettingsManager()
{
}

SettingsManager* SettingsManager::getInstance()
{
	static SettingsManager instance;
	return &instance;
}
