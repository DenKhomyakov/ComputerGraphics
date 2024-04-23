#ifndef OBSERVER_H
#define OBSERVER_H

#include "scene.h"
#include <QWidget>
#include <QLineEdit>
#include <QTimer>
#include <QPushButton>

class Observer: public QWidget {
    Q_OBJECT

private:
    QVector3D coordObserver;

    Scene scene;

    QLineEdit *inputX;
    QLineEdit *inputY;
    QLineEdit *inputZ;

    QTimer *timer;

    QPushButton *restart;

    qreal radius {0};
    qreal param {0};

    virtual void paintEvent(QPaintEvent *event);

public:
    Observer(qreal x = 0, qreal y = 1, qreal z = 0, QWidget *parent = nullptr);

public slots:
    void newCoordObserver();
    void startOrStop();
    void movingObserver();
};

#endif // OBSERVER_H
