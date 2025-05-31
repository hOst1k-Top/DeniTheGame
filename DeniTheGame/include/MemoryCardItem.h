#pragma once

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include "GameManager.h"


class MemoryCardItem : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT

public:
	MemoryCardItem(int id, QGraphicsItem* parent = nullptr);
    int getId() const;
    bool isFaceUp() const;

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private:
    void updatePixmap();

    int m_id;
    bool m_faceUp;
    QPointF m_dragStartPos;
};
