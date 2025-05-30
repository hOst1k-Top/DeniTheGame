#pragma once

#include <QLabel>
#include <QMouseEvent>
#include <QWidget>

enum class State
{
	Selected,
	Unselected
};

class ClickableCard : public QLabel
{
	Q_OBJECT

public:
	ClickableCard(QWidget *parent);
	~ClickableCard();
signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent* event) override;

private:
	State currentState;
};
