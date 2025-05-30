#include "include/ClickableCard.h"

ClickableCard::ClickableCard(QWidget *parent)
	: QLabel(parent)
{
}

ClickableCard::~ClickableCard()
{
}

void ClickableCard::mousePressEvent(QMouseEvent* event)
{
	emit clicked();
}
