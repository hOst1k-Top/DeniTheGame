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
	State getState() const { return currentState; }
	void resetState();
	int getId() const { return cardId; }
	void setId(int id) { cardId = id; };
	void setInteraction(bool isActive) { interactionEnabled = isActive; }

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent* event) override;

private:
	State currentState;
	int cardId;
	bool interactionEnabled = true;
	void updateDisplay();
};
