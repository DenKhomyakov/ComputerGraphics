#ifndef OBSERVER_H
#define OBSERVER_H

#include "scene.h"
#include <QWidget>
#include <QLineEdit>
#include <QTimer>
#include <QPushButton>

/*
 Класс Observer предоставляет функционал для наблюдения за сценой из определенной точки пространства и ее визуализации на виджете.
 Он также предоставляет возможность изменять координаты наблюдателя и анимировать его движение
*/

class Observer: public QWidget {
    Q_OBJECT

private:
    QVector3D coordObserver;                                                    // Координаты наблюдателя в трехмерном пространстве

    Scene scene;                                                                // Экземпляр класса Scene, представляющий сцену, которую наблюдает наблюдатель

    QLineEdit *inputX;                                                          /* Поля ввода для изменения координат наблюдателя */
    QLineEdit *inputY;
    QLineEdit *inputZ;

    QTimer *timer;                                                              // Таймер, используемый для анимации движения наблюдателя

    QPushButton *restart;                                                       // Кнопка для перезапуска анимации

    qreal radius {0};                                                           /* Вспомогательные параметры для вычисления координат наблюдателя при анимации */
    qreal param {0};

    virtual void paintEvent(QPaintEvent *event);                                // Метод paintEvent переопределен для визуализации сцены наблюдателем. Он использует метод drawFor класса Scene для рисования сцены на виджете

public:
    Observer(qreal x = 0, qreal y = 1, qreal z = 0, QWidget *parent = nullptr); // Конструктор класса, принимающий начальные координаты наблюдателя и родительский виджет

public slots:
    void newCoordObserver();                                                    // Слот для обновления координат наблюдателя из полей ввода
    void startOrStop();                                                         // Слот для запуска или остановки анимации движения наблюдателя
    void movingObserver();                                                      // Слот для обновления координат наблюдателя при анимации
};

#endif // OBSERVER_H
