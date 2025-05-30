#pragma once

#include <QString>

enum class Role {
	Unknown,
	Dany,
	Alter
};

class Player
{
public:
	Player();
	Player(QString username, int identifier);
	~Player();
	QString name;
	int id;
	std::vector<int> currentHend;
	Role role = Role::Unknown;
};
