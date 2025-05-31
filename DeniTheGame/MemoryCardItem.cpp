#include "include/MemoryCardItem.h"

MemoryCardItem::MemoryCardItem(int id, QGraphicsItem* parent)
	: QGraphicsPixmapItem(parent), 
	m_id(id), 
	m_faceUp(true)
{
	setPixmap(GameManager::getMemoryCardImage(m_id).scaled(200, 300));
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
	setTransformationMode(Qt::SmoothTransformation);
    setTransformOriginPoint(boundingRect().center());
	setAcceptHoverEvents(true);
}

int MemoryCardItem::getId() const
{
    return m_id;
}

bool MemoryCardItem::isFaceUp() const
{
    return m_faceUp;
}

void MemoryCardItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    m_faceUp = !m_faceUp;
    updatePixmap();
}

void MemoryCardItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        m_dragStartPos = event->pos();
    QGraphicsPixmapItem::mousePressEvent(event);
}

void MemoryCardItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsPixmapItem::mouseMoveEvent(event);
}

void MemoryCardItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    double angle = event->delta() / 8.0;
    setRotation(rotation() + angle);
    event->accept();
}

void MemoryCardItem::updatePixmap()
{
    if (m_faceUp)
        setPixmap(GameManager::getMemoryCardImage(m_id).scaled(200, 300));
    else
        setPixmap(QPixmap(200, 300));
    if (!m_faceUp)
        pixmap().fill(Qt::black);
}
