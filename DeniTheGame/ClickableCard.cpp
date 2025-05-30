#include "include/ClickableCard.h"

ClickableCard::ClickableCard(QWidget *parent)
	: QLabel(parent)
{
}

ClickableCard::~ClickableCard()
{
}

void ClickableCard::resetState()
{
	currentState = State::Unselected;
	updateDisplay();
}

void ClickableCard::updateDisplay()
{
	switch (currentState)
	{
	case State::Selected:
		setStyleSheet("QWidget { border: 2px solid cyan; border-radius: 5px; }");
		break;
	case State::Unselected:
		setStyleSheet("");
		break;
	default:
		break;
	}
}

void ClickableCard::mousePressEvent(QMouseEvent* event)
{
	emit clicked();
	if(interactionEnabled)
	{
		if (currentState == State::Selected) currentState = State::Unselected;
		else currentState = State::Selected;
		updateDisplay();
	}
}
