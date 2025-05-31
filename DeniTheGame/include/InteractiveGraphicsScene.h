#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class InteractiveGraphicsScene : public QGraphicsScene
{
	Q_OBJECT

public:
	InteractiveGraphicsScene(QObject *parent);
    void setInteractionEnabled(bool enabled);
    bool isInteractionEnabled() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    bool m_interactionEnabled;
};
