#include "include/InteractiveGraphicsScene.h"

InteractiveGraphicsScene::InteractiveGraphicsScene(QObject *parent)
	: QGraphicsScene(parent), 
	m_interactionEnabled(true)
{
}

void InteractiveGraphicsScene::setInteractionEnabled(bool enabled)
{
    m_interactionEnabled = enabled;
}

bool InteractiveGraphicsScene::isInteractionEnabled() const
{
    return m_interactionEnabled;
}

void InteractiveGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_interactionEnabled)
        QGraphicsScene::mousePressEvent(event);
    else
        event->ignore();
}

void InteractiveGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_interactionEnabled)
        QGraphicsScene::mouseDoubleClickEvent(event);
    else
        event->ignore();
}
